#include "sftp_client.h"
#include <fstream>
#include <string.h>
#include <cstring>
#include "unilog.h"

#define SFTP_PORT 22

SftpClient::SftpClient(const std::string& host, const std::string& user, const std::string& passwd)
    : host_(host),
    user_(user),
    passwd_(passwd)
{
    LOG_INFO_STM("SftpClient::SftpClient(), host:" << host_ << ", user:" << user_ << ", passwd:" << passwd_);
}

SftpClient::~SftpClient()
{
    LOG_INFO_STM("SftpClient::~SftpClient(), host:" << host_ << ", user:" << user_ << ", passwd:" << passwd_);
}

bool SftpClient::upload(const std::string& local_file, const std::string& remote_file)
{
    if (!init())
    {
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
                    LOG_ERROR_STM("failed to write remote file: " << remote_file << ", written:" << written << ", need send:" << in.gcount() - snd_cnt);
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
        clear();
    }

    LOG_INFO_STM("upload local_file:" << local_file << ", remote_file:" << remote_file << ", ret:"
                 << ret);
    return ret;
}


bool SftpClient::download(const std::string& remote_file, const std::string& local_file)
{
    if (!init())
    {
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
            LOG_ERROR_STM("Error reading from remote file, n:" << n);
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
        clear();
    }

    LOG_INFO_STM("Downloaded: " << remote_file << " -> " << local_file << ", ret:" << ret);
    return ret;
}

bool SftpClient::list_files(const std::string& remote_path, std::vector<std::string>& files)
{
    if (!init())
    {
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
            // note: 只返回文件，不返回路径
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
        clear();
    }

    return ret;
}

void SftpClient::init_sftp_lib()
{
    libssh2_init(0);
}

void SftpClient::deinit_sftp_lib()
{
    libssh2_exit();
}

bool SftpClient::init()
{
    if (is_init_)
    {
        return true;
    }

    do
    {
        // 创建 TCP socket
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd_ < 0)
        {
            LOG_ERROR_STM("create scoket failed.");
            break;
        }

        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(SFTP_PORT);
        if (inet_pton(AF_INET, host_.c_str(), &sin.sin_addr) <= 0)
        {
            LOG_ERROR_STM("inet pton failed, host:" << host_ << ", port:" << SFTP_PORT);
            break;
        }

        if (connect(socket_fd_, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0)
        {
            LOG_ERROR_STM("connect failed, host:" << host_ << ", port:" << SFTP_PORT);
            break;
        }

        // 创建 SSH session
        session_ = libssh2_session_init();
        if (!session_)
        {
            LOG_ERROR_STM("create session failed.");
            break;
        }

        if (libssh2_session_handshake(session_, socket_fd_) < 0)
        {
            LOG_ERROR_STM("session handshake failed. host:" << host_ << ", port:" << SFTP_PORT);
            break;
        }

        if (libssh2_userauth_password(session_, user_.c_str(), passwd_.c_str()) < 0)
        {
            LOG_ERROR_STM("user auth failed, user:" << user_ << ", passwd:" << passwd_ << ", host:"
                         << host_ << ", port:" << SFTP_PORT);
            break;
        }

        // 启动 SFTP
        sftp_ = libssh2_sftp_init(session_);
        if (!sftp_)
        {
            LOG_ERROR_STM("sftp init failed. host:" << host_ << ", port:" << SFTP_PORT);
            break;
        }

        is_init_ = true;
        LOG_INFO_STM("SftpClient::init success, host:" << host_ << ", user:" << user_ << ", passwd:" << passwd_);

    } while (false);

    if (!is_init_)
    {
        clear();
    }

    return is_init_;
}

void SftpClient::clear()
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

    LOG_INFO_STM("ftp client clear. host:" << host_ << ", user:" << user_ << ", passwd:" << passwd_);
}
