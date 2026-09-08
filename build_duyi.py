import asyncio
import json
import paramiko
import os
import uuid
import shlex
from pathlib import Path
from mcp.server.mcpserver import MCPServer

# ---------------------- 初始化MCP服务 ----------------------
mcp = MCPServer("remote-build-mcp")

# 缓存SSH会话字典 key:session_id, value:paramiko客户端对象
ssh_sessions: dict[str, paramiko.SSHClient] = {}

# 项目根目录（脚本所在目录）
PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))


# ---------------------- 从 sftp.json 读取默认配置 ----------------------
def _load_sftp_config() -> dict:
    """读取 .vscode/sftp.json 获取远程服务器默认配置。"""
    sftp_path = os.path.join(PROJECT_ROOT, ".vscode", "sftp.json")
    try:
        with open(sftp_path, "r", encoding="utf-8") as f:
            cfg = json.load(f)
        return {
            "host": cfg.get("host", ""),
            "port": int(cfg.get("port", 22)),
            "username": cfg.get("username", ""),
            "password": cfg.get("password", ""),
            "remotePath": cfg.get("remotePath", ""),
        }
    except Exception as e:
        print(f"警告：读取 sftp.json 失败 - {e}")
        return {}


SFTP_CONFIG = _load_sftp_config()
LOCAL_DIR_DEFAULT = PROJECT_ROOT
REMOTE_DIR_DEFAULT = SFTP_CONFIG.get("remotePath", "/home/mcgs/Desktop/DuySorter_rk3566_simple")
# 默认排除项：编译产物 + 不需要同步的目录（app/ 存放编译后的二进制和运行时库，无需同步源码）
EXCLUDE_DEFAULT = (
    ".git,__pycache__,.vscode,.DS_Store,"
    "build,rel,app,.qmake.stash,*.user,Makefile,Makefile.Debug,Makefile.Release"
)


# ---------------------- 板卡 (RK3568 目标机) 配置 ----------------------
BOARD_CONFIG = {
    "host": "192.168.3.52",
    "port": 22,
    "username": "root",
    "password": "duyi",
    "work_dir": "/root/test",
}

# 板卡 SSH 会话缓存 (key: board_session_id)
board_sessions: dict[str, paramiko.SSHClient] = {}

# 独立测试程序清单: 编译产物相对路径 + 板卡运行名
TEST_BINARIES = {
    "camera_uart_capture": {
        "vm_dir": "test/camera_uart_capture",
        "vm_bin": "arm64/camera_uart_capture_test_arm64",
        "board_bin": "camera_uart_capture_test_arm64",
    },
    "camera_board_check": {
        "vm_dir": "test/camera_board_check",
        "vm_bin": "arm64/camera_board_check_arm64",
        "board_bin": "camera_board_check_arm64",
    },
}


# ---------------------- 内部辅助函数 ----------------------
def _run_remote(client: paramiko.SSHClient, full_cmd: str) -> str:
    """执行远程命令，返回统一格式的 stdout/stderr/exit_code 日志。"""
    stdin, stdout, stderr = client.exec_command(full_cmd)
    exit_code = stdout.channel.recv_exit_status()

    out_text = stdout.read().decode("utf-8", errors="ignore")
    err_text = stderr.read().decode("utf-8", errors="ignore")

    return (
        f"===== EXIT_CODE:{exit_code} =====\n"
        f"===== STDOUT =====\n{out_text}\n"
        f"===== STDERR(报错) =====\n{err_text}"
    )


def _run_board(client: paramiko.SSHClient, full_cmd: str, timeout: int) -> str:
    """在板卡上执行命令，带 timeout 限时（适合会循环运行/可能卡住的程序）。"""
    # 用 sh -c 包一层，让 timeout 作用到整个 && 链，而不是只作用到第一个命令
    wrapped = f"timeout {timeout} sh -c {shlex.quote(full_cmd)}"
    stdin, stdout, stderr = client.exec_command(wrapped)
    exit_code = stdout.channel.recv_exit_status()

    out_text = stdout.read().decode("utf-8", errors="ignore")
    err_text = stderr.read().decode("utf-8", errors="ignore")

    note = " (timeout 限时结束, 属预期)" if exit_code == 124 else ""

    return (
        f"===== EXIT_CODE:{exit_code}{note} =====\n"
        f"===== STDOUT =====\n{out_text}\n"
        f"===== STDERR(报错) =====\n{err_text}"
    )


def _mkdirs(sftp, remote_dir: str) -> None:
    """逐级创建远程目录，目录已存在则跳过。"""
    if not remote_dir or remote_dir in ("/", "."):
        return
    cur = "/" if remote_dir.startswith("/") else ""
    for part in remote_dir.strip("/").split("/"):
        if not part:
            continue
        cur = f"{cur}/{part}" if cur else part
        try:
            sftp.mkdir(cur)
        except IOError:
            pass


def _should_exclude(name: str, exclude_patterns: set[str]) -> bool:
    """判断文件/目录名是否应被排除（支持通配符前缀如 *.o）。"""
    if name in exclude_patterns:
        return True
    for pat in exclude_patterns:
        if pat.startswith("*") and name.endswith(pat[1:]):
            return True
        if pat.endswith("*") and name.startswith(pat[:-1]):
            return True
    return False


def _upload_dir(sftp, local_dir: str, remote_dir: str, exclude: set[str]) -> int:
    """递归上传本地目录到远程，返回实际上传的文件数。"""
    if not os.path.isdir(local_dir):
        return 0
    _mkdirs(sftp, remote_dir)
    uploaded = 0
    for name in sorted(os.listdir(local_dir)):
        if _should_exclude(name, exclude):
            continue
        local_path = os.path.join(local_dir, name)
        remote_path = remote_dir.rstrip("/") + "/" + name
        if os.path.isdir(local_path):
            uploaded += _upload_dir(sftp, local_path, remote_path, exclude)
        else:
            sftp.put(local_path, remote_path)
            uploaded += 1
    return uploaded


# ----------------------工具1：ssh_connect 建立远程会话----------------------
@mcp.tool()
async def ssh_connect(
    host: str = "",
    port: int = 0,
    username: str = "",
    work_dir: str = "",
    private_key_path: str = "",
    password: str = "",
) -> str:
    """
    登录远程编译服务器，缓存SSH会话（参数留空则自动从 sftp.json 读取）。
    :param host: 服务器IP地址（默认从sftp.json读取）
    :param port: SSH端口，默认22
    :param username: 远程服务器用户名（默认从sftp.json读取）
    :param work_dir: 编译项目根目录，后续命令都在此目录执行（默认使用 sftp.json 的 remotePath）
    :param private_key_path: 本地私钥绝对路径（可选，填了则用私钥，忽略密码）
    :param password: 登录密码（默认从sftp.json读取，未填私钥时使用）
    :return: session_id会话标识
    """
    # 从 sftp.json 填充默认值
    host = host or SFTP_CONFIG.get("host", "")
    port = port or SFTP_CONFIG.get("port", 22)
    username = username or SFTP_CONFIG.get("username", "")
    password = password or SFTP_CONFIG.get("password", "")
    work_dir = work_dir or REMOTE_DIR_DEFAULT

    if not host or not username:
        return "错误：无法获取 host/username，请检查 sftp.json 或手动传入"

    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    connect_kwargs = dict(hostname=host, port=port, username=username, timeout=15)
    if private_key_path:
        key_path = os.path.expanduser(private_key_path)
        try:
            key = paramiko.RSAKey.from_private_key_file(key_path)
        except Exception:
            key = paramiko.Ed25519Key.from_private_key_file(key_path)
        connect_kwargs["pkey"] = key
    elif password:
        connect_kwargs["password"] = password
    else:
        return "错误：private_key_path 和 password 至少提供一个"

    client.connect(**connect_kwargs)
    session_id = str(uuid.uuid4())
    client._work_dir = work_dir
    ssh_sessions[session_id] = client
    return f"SSH连接成功，session_id={session_id}，工作目录:{work_dir}"


# ----------------------工具2：upload_files 同步本地代码到服务器----------------------
@mcp.tool()
async def upload_files(
    session_id: str,
    local_dir: str = "",
    remote_dir: str = "",
    exclude: str = EXCLUDE_DEFAULT,
) -> str:
    """
    递归上传本地目录到远程服务器（SFTP），用于代码修改后同步到编译机。
    :param session_id: ssh_connect 返回的会话ID
    :param local_dir: 本地目录绝对路径（默认脚本所在项目根目录）
    :param remote_dir: 远程目录绝对路径（默认使用 ssh_connect 时的工作目录）
    :param exclude: 逗号分隔的排除项（文件或目录名，支持通配符如 *.o），默认排除编译产物
    :return: 上传结果统计
    """
    if session_id not in ssh_sessions:
        return "错误：会话不存在，请重新调用ssh_connect"

    client = ssh_sessions[session_id]
    local_dir = local_dir or LOCAL_DIR_DEFAULT
    remote_dir = remote_dir or client._work_dir
    exclude_set = {x.strip() for x in exclude.split(",") if x.strip()}

    sftp = client.open_sftp()
    try:
        n = _upload_dir(sftp, local_dir, remote_dir, exclude_set)
    finally:
        sftp.close()
    return f"上传完成：{n} 个文件 -> {remote_dir}"


# ----------------------工具3：build_duyi 远程编译 DuySorter ----------------------
@mcp.tool()
async def build_duyi(
    session_id: str,
    work_dir: str = "",
    mode: str = "release",
    product: str = "6SXZ-600G_335X",
) -> str:
    """
    在远程服务器编译 DuySorter 项目。
    :param session_id: ssh_connect 返回的会话ID
    :param work_dir: 项目根目录（默认 ssh_connect 时的工作目录）
    :param mode: 编译模式 release/debug/clean，默认 release
    :param product: 产品型号，默认 6SXZ-600G_335X
    :return: 编译日志 + exit_code（非 0 表示编译失败，需根据 STDERR 修改代码后重新上传+编译）
    """
    if session_id not in ssh_sessions:
        return "错误：会话不存在，请重新调用ssh_connect"

    client = ssh_sessions[session_id]
    wd = work_dir or client._work_dir
    full_cmd = f"cd {wd} && ./qtmakeApp.sh {mode} {product}"
    return _run_remote(client, full_cmd)


# ----------------------工具4：remote_run_build 执行任意编译命令----------------------
@mcp.tool()
async def remote_run_build(session_id: str, build_command: str) -> str:
    """
    在远程服务器执行编译命令，捕获stdout、stderr、退出码
    :param session_id: ssh_connect返回的会话ID
    :param build_command: 编译指令，如 make -j8, cmake --build .
    :return:完整日志+exit_code
    """
    if session_id not in ssh_sessions:
        return "错误：会话不存在，请重新调用ssh_connect"

    client = ssh_sessions[session_id]
    wd = client._work_dir
    full_cmd = f"cd {wd} && {build_command}"

    return _run_remote(client, full_cmd)


# ----------------------工具5：remote_edit_file 修改远程源码----------------------
@mcp.tool()
async def remote_edit_file(session_id: str, file_path: str, new_content: str) -> str:
    """
    修改远程服务器源代码文件(SFTP覆盖写入)
    :param session_id:会话id
    :param file_path:远程服务器上源码绝对路径
    :param new_content:修改完成后的完整源代码文本
    """
    if session_id not in ssh_sessions:
        return "错误：会话不存在，请重新调用ssh_connect"
    client = ssh_sessions[session_id]
    try:
        sftp = client.open_sftp()
        with sftp.open(file_path, "w", encoding="utf-8") as f:
            f.write(new_content)
        sftp.close()
        return f"文件写入成功:{file_path}"
    except Exception as e:
        return f"写入失败:{str(e)}"


# ----------------------工具6：read_remote_file 读取远程源码----------------------
@mcp.tool()
async def read_remote_file(session_id: str, file_path: str) -> str:
    """读取远程服务器源码文件，Agent修改前先获取原始代码"""
    if session_id not in ssh_sessions:
        return "错误：会话不存在，请重新调用ssh_connect"
    client = ssh_sessions[session_id]
    try:
        sftp = client.open_sftp()
        with sftp.open(file_path, "r") as f:
            content = f.read().decode("utf-8", errors="ignore")
        sftp.close()
        return content
    except Exception as e:
        return f"读取失败:{str(e)}"


# ----------------------工具7：board_connect 连接板卡 ----------------------
@mcp.tool()
async def board_connect(
    host: str = "",
    port: int = 0,
    username: str = "",
    password: str = "",
    work_dir: str = "",
) -> str:
    """
    登录 RK3568 板卡（默认 192.168.4.2, root/duyi, 运行目录 /root/test）。
    :param host: 板卡IP (默认 192.168.4.2)
    :param port: SSH端口 (默认 22)
    :param username: 板卡用户名 (默认 root)
    :param password: 板卡密码 (默认 duyi)
    :param work_dir: 板卡上程序运行目录 (默认 /root/test)
    :return: board_session_id
    """
    host = host or BOARD_CONFIG["host"]
    port = port or BOARD_CONFIG["port"]
    username = username or BOARD_CONFIG["username"]
    password = password or BOARD_CONFIG["password"]
    work_dir = work_dir or BOARD_CONFIG["work_dir"]

    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client.connect(hostname=host, port=port, username=username, password=password, timeout=15)
    client._work_dir = work_dir
    session_id = "board-" + str(uuid.uuid4())
    board_sessions[session_id] = client
    return f"板卡连接成功，board_session_id={session_id}，工作目录:{work_dir}"


# ----------------------工具8：vm_download_file 从编译机下载产物到本地 ----------------------
@mcp.tool()
async def vm_download_file(session_id: str, remote_path: str, local_path: str = "") -> str:
    """
    从编译机下载文件到本地 (SFTP get)。
    :param session_id: 编译机 ssh_connect 返回的会话ID
    :param remote_path: 编译机上文件绝对路径 (如 /home/mcgs/.../arm64/camera_uart_capture_test_arm64)
    :param local_path: 本地保存路径 (默认按项目根目录镜像 remote_path)
    :return: 下载结果
    """
    if session_id not in ssh_sessions:
        return "错误：编译机会话不存在，请重新调用ssh_connect"
    client = ssh_sessions[session_id]
    local_path = local_path or os.path.join(PROJECT_ROOT, remote_path.lstrip("/"))
    try:
        os.makedirs(os.path.dirname(local_path), exist_ok=True)
        sftp = client.open_sftp()
        sftp.get(remote_path, local_path)
        sftp.close()
        size = os.path.getsize(local_path)
        return f"下载完成: {remote_path} -> {local_path} ({size} bytes)"
    except Exception as e:
        return f"下载失败: {str(e)}"


# ----------------------工具9：board_upload_file 上传文件到板卡 ----------------------
@mcp.tool()
async def board_upload_file(board_session_id: str, local_path: str, remote_name: str = "") -> str:
    """
    上传本地文件到板卡运行目录 (SFTP put) 并自动 chmod +x。
    :param board_session_id: board_connect 返回的会话ID
    :param local_path: 本地文件绝对路径
    :param remote_name: 板卡上的文件名 (默认用本地文件名)
    :return: 上传结果
    """
    if board_session_id not in board_sessions:
        return "错误：板卡会话不存在，请重新调用board_connect"
    client = board_sessions[board_session_id]
    remote_name = remote_name or os.path.basename(local_path)
    remote_path = client._work_dir.rstrip("/") + "/" + remote_name
    try:
        sftp = client.open_sftp()
        sftp.put(local_path, remote_path)
        sftp.close()
        _run_remote(client, f"chmod +x {remote_path}")
        return f"上传完成: {local_path} -> {remote_path}"
    except Exception as e:
        return f"上传失败: {str(e)}"


# ----------------------工具10：board_run 在板卡上运行命令 ----------------------
@mcp.tool()
async def board_run(board_session_id: str, command: str = "", timeout: int = 20) -> str:
    """
    在板卡运行目录执行命令 (自动 cd 到 work_dir, 用 timeout 限时)。
    :param board_session_id: board_connect 返回的会话ID
    :param command: 要执行的命令 (留空默认 ./camera_uart_capture_test_arm64)
    :param timeout: 限时秒数, 超时用 timeout 杀掉并返回已捕获输出
    :return: stdout/stderr/exit_code (124 表示 timeout 限时结束)
    """
    if board_session_id not in board_sessions:
        return "错误：板卡会话不存在，请重新调用board_connect"
    client = board_sessions[board_session_id]
    command = command or "./camera_uart_capture_test_arm64"
    full_cmd = f"cd {client._work_dir} && {command}"
    return _run_board(client, full_cmd, timeout)


# ----------------------工具11：deploy_test 一键编译部署运行测试程序 ----------------------
@mcp.tool()
async def deploy_test(
    vm_session_id: str,
    board_session_id: str,
    test_name: str = "camera_uart_capture",
    run_args: str = "",
    run_timeout: int = 20,
    do_build: bool = True,
) -> str:
    """
    一键流程: 编译机编译测试程序 → 下载到本地 → 上传到板卡 → 运行并拉回结果。
    :param vm_session_id: 编译机 ssh_connect 会话ID (源码需先 upload_files 同步到编译机)
    :param board_session_id: 板卡 board_connect 会话ID
    :param test_name: camera_uart_capture 或 camera_board_check
    :param run_args: 传给测试程序的命令行参数 (如 "/dev/ttyS4 921600 1 1 0")
    :param run_timeout: 板卡运行限时秒数
    :param do_build: 是否先编译 (False 则跳过编译直接用已下载产物)
    :return: 编译/下载/上传/运行 各阶段日志
    """
    if vm_session_id not in ssh_sessions:
        return "错误：编译机会话不存在，请重新调用ssh_connect"
    if board_session_id not in board_sessions:
        return "错误：板卡会话不存在，请重新调用board_connect"

    vm = ssh_sessions[vm_session_id]
    board = board_sessions[board_session_id]

    info = TEST_BINARIES.get(test_name)
    if not info:
        return f"错误：未知 test_name={test_name}, 可选 {list(TEST_BINARIES)}"

    vm_dir = info["vm_dir"]
    vm_bin = info["vm_bin"]
    board_bin = info["board_bin"]
    parts = []

    # 1. 编译
    if do_build:
        vm_full = vm._work_dir.rstrip("/") + "/" + vm_dir
        build_result = _run_remote(vm, f"cd {vm_full} && ./build.sh arm")
        parts.append(f"===== [1/4] 编译机编译 =====\n{build_result}")
        if "===== EXIT_CODE:0 =====" not in build_result:
            parts.append("编译失败，终止后续步骤。请根据 STDERR 修改代码后重新 upload_files + deploy_test。")
            return "\n\n".join(parts)

    # 2. 下载到本地
    vm_bin_abs = vm._work_dir.rstrip("/") + "/" + vm_dir + "/" + vm_bin
    local_bin = os.path.join(PROJECT_ROOT, vm_dir, vm_bin)
    try:
        os.makedirs(os.path.dirname(local_bin), exist_ok=True)
        sftp = vm.open_sftp()
        sftp.get(vm_bin_abs, local_bin)
        sftp.close()
        size = os.path.getsize(local_bin)
        parts.append(f"===== [2/4] 下载到本地 =====\n{vm_bin_abs} -> {local_bin} ({size} bytes)")
    except Exception as e:
        parts.append(f"===== [2/4] 下载失败 =====\n{str(e)}")
        return "\n\n".join(parts)

    # 3. 上传到板卡
    board_path = board._work_dir.rstrip("/") + "/" + board_bin
    try:
        sftp = board.open_sftp()
        sftp.put(local_bin, board_path)
        sftp.close()
        _run_remote(board, f"chmod +x {board_path}")
        parts.append(f"===== [3/4] 上传到板卡 =====\n{local_bin} -> {board_path}")
    except Exception as e:
        parts.append(f"===== [3/4] 上传失败 =====\n{str(e)}")
        return "\n\n".join(parts)

    # 4. 运行并拉回结果
    run_cmd = f"./{board_bin} {run_args}".strip()
    run_result = _run_board(board, f"cd {board._work_dir} && {run_cmd}", run_timeout)
    parts.append(f"===== [4/4] 板卡运行 =====\n{run_result}")

    return "\n\n".join(parts)


if __name__ == "__main__":
    # stdio模式，给TRAE调用
    mcp.run(transport="stdio")
