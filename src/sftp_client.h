/*
 * @Description: SFTP 客户端（libssh2 原生，持久 session）
 * 用法：
 *   SftpClient cli(host, port, user, passwd);
 *   cli.connect();                    // 一次握手
 *   cli.mkdir_p("/ftp/task/raw/image");
 *   cli.upload("local/a.jpg", "/ftp/task/raw/image/a.jpg");
 *   cli.upload("local/b.jpg", "/ftp/task/raw/image/b.jpg");  // 复用 session
 *   cli.disconnect();                 // 一次断连
 *
 * @Author: xiekai + aimodel team
 */
#ifndef __SFTP_CLIENT_H__
#define __SFTP_CLIENT_H__

#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <libssh2.h>
#include <libssh2_sftp.h>

class SftpClient final
{
public:
    SftpClient(const std::string& host, int port,
               const std::string& user, const std::string& passwd);
    ~SftpClient();

    /**
     * @brief: 建立 SSH + SFTP 连接（一次握手即可复用）
     */
    bool connect();

    /**
     * @brief: 断开连接（析构自动调，也可手动提前释放）
     */
    void disconnect();

    /**
     * @brief: 检查当前 session 是否已连接
     */
    bool isConnected() const { return is_init_; }

    /**
     * @brief: 上传单个文件（remote_file 必须是完整路径，如 /ftp/task/raw/image/a.jpg）
     */
    bool upload(const std::string& local_file, const std::string& remote_file);

    /**
     * @brief: 下载单个文件
     */
    bool download(const std::string& remote_file, const std::string& local_file);

    /**
     * @brief: 遍历远程目录下的文件（只返回文件名，不含路径）
     */
    bool list_files(const std::string& remote_path, std::vector<std::string>& files);

    /**
     * @brief: 创建单层远程目录（已存在返回 true）
     */
    bool mkdir(const std::string& remote_dir, int mode = 0755);

    /**
     * @brief: 递归创建远程目录（类似 mkdir -p）
     */
    bool mkdir_p(const std::string& remote_dir, int mode = 0755);

    /**
     * @brief: SFTP stat 远程文件（不跟随符号链接）
     * @return true 存在且 attrs 填充；false 不存在或 stat 失败
     */
    bool stat_file(const std::string& remote_file, LIBSSH2_SFTP_ATTRIBUTES& attrs);

    /**
     * @brief: SSH exec 远程命令，返回 stdout
     * @param cmd shell 命令字符串（如 "md5sum /ftp/model/a.bin"）
     * @return stdout 内容；失败返回空串
     */
    std::string exec(const std::string& cmd);

    /** libssh2 全局初始化 / 释放（程序启动/退出各一次） */
    static void init_sftp_lib();
    static void deinit_sftp_lib();

private:
    // 构造后不能用 init()/clear() 了，改用 connect()/disconnect()

    bool is_init_{ false };
    std::string host_;
    int         port_{ 22 };
    std::string user_;
    std::string passwd_;

    int socket_fd_{ -1 };
    LIBSSH2_SESSION* session_{ nullptr };
    LIBSSH2_SFTP* sftp_{ nullptr };
};


#endif // __SFTP_CLIENT_H__
