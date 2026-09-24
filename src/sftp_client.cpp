#include "sftp_client.h"
#include <fstream>
#include <string.h>
#include <cstring>
#include <netdb.h>
#include "unilog.h"

SftpClient::SftpClient(const std::string& host, int port,
    const std::string& user, const std::string& passwd)
    : host_(host), port_(port), user_(user), passwd_(passwd)
{
    LOG_INFO_STM("SftpClient ctor: " << host_ << ":" << port_ << " user=" << user_);
}

SftpClient::~SftpClient()
{
    disconnect();
    LOG_INFO_STM("SftpClient dtor: " << host_ << ":" << port_);
}


bool SftpClient::connect()
{
    if (is_init_)
    {
        return true;
    }

    do
    {
        // DNS 解析（支持域名 / IP）
        struct addrinfo hints { 0 };
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        char portbuf[16];
        snprintf(portbuf, sizeof(portbuf), "%d", port_);
        struct addrinfo* res{ nullptr };

        if (getaddrinfo(host_.c_str(), portbuf, &hints, &res) != 0 || !res)
        {
            LOG_ERROR_STM("getaddrinfo failed, host:" << host_ << ", port:" << port_);
            break;
        }

        // TCP socket + connect（遍历 addrinfo 直到成功）
        bool tcp_ok = false;
        for (struct addrinfo* rp = res; rp; rp = rp->ai_next)
        {
            socket_fd_ = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (socket_fd_ < 0)
            {
                continue;
            }

            // 设置超时时间，防止网络不通
            struct timeval tv;
            tv.tv_sec = 3;
            tv.tv_usec = 0;
            setsockopt(socket_fd_,
                SOL_SOCKET,
                SO_SNDTIMEO,
                &tv,
                sizeof(tv));

            if (::connect(socket_fd_, rp->ai_addr, rp->ai_addrlen) == 0)
            {
                tcp_ok = true;
                break;
            }
            close(socket_fd_);
            socket_fd_ = -1;
        }

        freeaddrinfo(res);
        if (!tcp_ok)
        {
            LOG_ERROR_STM("connect failed, host:" << host_ << ", port:" << port_);
            break;
        }

        // SSH session
        session_ = libssh2_session_init();
        if (!session_)
        {
            LOG_ERROR_STM("libssh2_session_init failed.");
            break;
        }

        if (libssh2_session_handshake(session_, socket_fd_) < 0)
        {
            LOG_ERROR_STM("session handshake failed, host:" << host_ << ", port:" << port_);
            break;
        }

        if (libssh2_userauth_password(session_, user_.c_str(), passwd_.c_str()) < 0)
        {
            LOG_ERROR_STM("user auth failed, user:" << user_ << ", host:" << host_);
            break;
        }

        // SFTP init
        sftp_ = libssh2_sftp_init(session_);
        if (!sftp_)
        {
            LOG_ERROR_STM("sftp init failed, host:" << host_);
            break;
        }

        is_init_ = true;
        LOG_INFO_STM("SftpClient connect OK: " << host_ << ":" << port_);

    } while (false);

    if (!is_init_)
    {
        disconnect();
    }
    return is_init_;
}

void SftpClient::disconnect()
{
    if (sftp_)
    {
        libssh2_sftp_shutdown(sftp_);
        sftp_ = nullptr;
    }
    if (session_)
    {
        libssh2_session_disconnect(session_, "Normal Shutdown");
        libssh2_session_free(session_);
        session_ = nullptr;
    }
    if (socket_fd_ >= 0)
    {
        close(socket_fd_);
        socket_fd_ = -1;
    }
    is_init_ = false;
    LOG_INFO_STM("SftpClient disconnect: " << host_ << ":" << port_);
}

// ═══════════════════════════════════════════════════════════════════════
// 操作方法（全部假设已 connect()，失败不自动断连）
// ═══════════════════════════════════════════════════════════════════════

bool SftpClient::mkdir(const std::string& remote_dir, int mode)
{
    if (!connect())
    {
        LOG_ERROR_STM("mkdir: not connected, call connect() first");
        return false;
    }

    // 先 stat，已存在 → 直接 true
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    if (libssh2_sftp_stat(sftp_, remote_dir.c_str(), &attrs) == 0)
    {
        if (LIBSSH2_SFTP_S_ISDIR(attrs.permissions))
        {
            LOG_INFO_STM("mkdir: already exists: " << remote_dir);
            return true;
        }
        LOG_ERROR_STM("mkdir: path exists but is not a dir: " << remote_dir);
        return false;
    }

    int rc = libssh2_sftp_mkdir(sftp_, remote_dir.c_str(), mode);
    if (rc < 0)
    {
        LOG_ERROR_STM("mkdir failed: " << remote_dir << ", rc=" << rc);
        return false;
    }
    LOG_INFO_STM("mkdir OK: " << remote_dir);
    return true;
}

bool SftpClient::mkdir_p(const std::string& remote_dir, int mode)
{
    if (remote_dir.empty())
    {
        return false;
    }

    if (!connect())
    {
        LOG_ERROR_STM("mkdir: not connected, call connect() first");
        return false;
    }

    // 逐级 mkdir，逐段用 / 切割
    std::string partial;
    size_t start = (remote_dir[0] == '/') ? 1 : 0;

    for (size_t i = start; i <= remote_dir.size(); ++i)
    {
        if (i == remote_dir.size() || remote_dir[i] == '/')
        {
            if (i == start) continue;
            partial = remote_dir.substr(0, i);
            if (partial.empty()) continue;
            if (!mkdir(partial, mode))
            {
                LOG_ERROR_STM("mkdir_p failed at: " << partial);
                return false;
            }
        }
    }
    return true;
}

bool SftpClient::upload(const std::string& local_file, const std::string& remote_file)
{
    if (!connect())
    {
        LOG_ERROR_STM("upload: not connected, call connect() first");
        return false;
    }

    std::ifstream in(local_file.c_str(), std::ios::binary);
    if (!in)
    {
        LOG_ERROR_STM("failed to open local file: " << local_file);
        return false;
    }

    bool ret{ true };
    LIBSSH2_SFTP_HANDLE* handle{ nullptr };
    do
    {
        handle = libssh2_sftp_open(sftp_, remote_file.c_str(),
            LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC, 0644);
        if (!handle)
        {
            ret = false;
            LOG_ERROR_STM("failed to open remote file: " << remote_file);
            break;
        }

        char buffer[32768];
        while (ret && (in.read(buffer, sizeof(buffer)) || (in.gcount() > 0)))
        {
            int snd_cnt = 0;
            do
            {
                ssize_t written = libssh2_sftp_write(handle, buffer + snd_cnt, in.gcount() - snd_cnt);
                if (written < 0)
                {
                    ret = false;
                    LOG_ERROR_STM("failed to write remote file: " << remote_file << ", written:" << written);
                    break;
                }
                snd_cnt += written;
            } while (snd_cnt < in.gcount());
        }
    } while (false);

    in.close();
    if (nullptr != handle)
    {
        libssh2_sftp_close(handle);
    }

    if (!ret)
    {
        disconnect();
        LOG_ERROR_STM("upload FAILED local:" << local_file << " -> remote:" << remote_file);
    }
    return ret;
}

bool SftpClient::download(const std::string& remote_file, const std::string& local_file)
{
    if (!connect())
    {
        LOG_ERROR_STM("download: not connected, call connect() first");
        return false;
    }

    std::ofstream out;
    bool ret{ false };
    LIBSSH2_SFTP_HANDLE* handle{ nullptr };
    do
    {
        handle = libssh2_sftp_open(sftp_, remote_file.c_str(), LIBSSH2_FXF_READ, 0);
        if (!handle)
        {
            LOG_ERROR_STM("failed to open remote file: " << remote_file);
            break;
        }

        out.open(local_file.c_str(), std::ios::binary);
        if (!out.is_open())
        {
            LOG_ERROR_STM("failed to open local file: " << local_file);
            break;
        }

        char buffer[32768];
        ssize_t n;
        while ((n = libssh2_sftp_read(handle, buffer, sizeof(buffer))) > 0)
        {
            out.write(buffer, n);
        }

        if (n < 0)
        {
            LOG_ERROR_STM("read error, remote:" << remote_file);
            break;
        }

        ret = true;

    } while (false);

    out.close();
    if (nullptr != handle)
    {
        libssh2_sftp_close(handle);
    }

    if (!ret)
    {
        disconnect();
        LOG_ERROR_STM("download FAILED remote:" << remote_file << " -> local:" << local_file);
    }
    return ret;
}

bool SftpClient::list_files(const std::string& remote_path, std::vector<std::string>& files)
{
    if (!connect())
    {
        LOG_ERROR_STM("list_files: not connected, call connect() first");
        return false;
    }

    bool ret{ false };
    LIBSSH2_SFTP_HANDLE* dir{ nullptr };
    do
    {
        dir = libssh2_sftp_opendir(sftp_, remote_path.c_str());
        if (!dir)
        {
            LOG_ERROR_STM("libssh2_sftp_opendir failed, path:" << remote_path);
            break;
        }

        LIBSSH2_SFTP_ATTRIBUTES attrs;
        char filename[256];
        while (libssh2_sftp_readdir(dir, filename, sizeof(filename), &attrs) > 0)
        {
            if (std::strcmp(filename, ".") == 0 || std::strcmp(filename, "..") == 0)
                continue;
            if (LIBSSH2_SFTP_S_ISDIR(attrs.permissions))
                continue;
            files.push_back(filename);
        }

        ret = true;
    } while (false);

    if (nullptr != dir)
    {
        libssh2_sftp_closedir(dir);
    }

    if (!ret)
    {
        disconnect();
    }

    return ret;
}

bool SftpClient::stat_file(const std::string& remote_file, LIBSSH2_SFTP_ATTRIBUTES& attrs)
{
    if (!connect())
    {
        LOG_ERROR_STM("stat_file: not connected, call connect() first");
        return false;
    }

    int rc = libssh2_sftp_stat(sftp_, remote_file.c_str(), &attrs);
    if (rc != 0)
    {
        // 文件不存在是正常情况，不算错误
        return false;
    }
    return true;
}

bool SftpClient::deleteFiles(const std::string& file)
{
    if (!connect())
    {
        LOG_ERROR_STM("deleteFiles: not connected, call connect() first");
        return false;
    }


    LOG_INFO_STM("delete ftp file:" << file);
    int rc = libssh2_sftp_unlink(sftp_, file.c_str());
    if (rc != 0)
    {
        // 文件不存在是正常情况，不算错误
        return false;
    }
    return true;
}

std::string SftpClient::exec(const std::string& cmd)
{
    if (!connect())
    {
        LOG_ERROR_STM("exec: not connected, call connect() first");
        return "";
    }

    LIBSSH2_CHANNEL* chan = libssh2_channel_open_session(session_);
    if (!chan)
    {
        LOG_ERROR_STM("exec: channel_open_session failed");
        return "";
    }

    if (libssh2_channel_exec(chan, cmd.c_str()) < 0)
    {
        LOG_ERROR_STM("exec: exec failed for cmd: " << cmd);
        libssh2_channel_close(chan);
        libssh2_channel_free(chan);
        return "";
    }

    // 读 stdout
    std::string output;
    char buf[4096];
    while (true)
    {
        ssize_t n = libssh2_channel_read(chan, buf, sizeof(buf));
        if (n > 0)
        {
            output.append(buf, n);
        }
        else if (n == 0)
        {
            break;  // EOF
        }
        else
        {
            LOG_ERROR_STM("exec: read failed, cmd=" << cmd);
            break;
        }
    }

    libssh2_channel_wait_closed(chan);
    libssh2_channel_close(chan);
    libssh2_channel_free(chan);

    LOG_INFO_STM("exec OK: " << cmd << " → " << output);
    return output;
}

// ═══════════════════════════════════════════════════════════════════════
// 全局 libssh2 初始化（程序启动/退出各一次）
// ═══════════════════════════════════════════════════════════════════════

void SftpClient::init_sftp_lib() { libssh2_init(0); }
void SftpClient::deinit_sftp_lib() { libssh2_exit(); }
