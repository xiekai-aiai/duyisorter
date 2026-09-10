/*
 * @Description:  ftp客户端头文件
 * @version:
 * @Author: xiekai
 * @Date: 2025-12-24 09:44:37
 * @LastEditors: xiekai
 * @LastEditTime: 2025-12-24 09:58:50
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
    SftpClient(const std::string& host, const std::string& user, const std::string& passwd);
    ~SftpClient();

    /**
     * @brief: 上传文件到远程ftp服务器
     * @param {string&} local_file  本地文件路径
     * @param {string&} remote_file 远程文件路径
     * @return {*}
     */
    bool upload(const std::string& local_file, const std::string& remote_file);

    /**
     * @brief: 下载文件到本地
     * @param {string&} remote_file 远程文件路径
     * @param {string&} local_file  本地文件路径
     * @return {*}
     */
    bool download(const std::string& remote_file, const std::string& local_file);

    /**
     * @brief: 遍历远程目录下的文件
     * @param {string&} remote_path  远程目录路径
     * @param {vector<string>&} files  文件列表
     * @return {*}
     */
    bool list_files(const std::string& remote_path, std::vector<std::string>& files);

    /**
     * @brief: 初始化sftp库, 程序启动时调用一次即可
     * @return {*}
     */
    static void init_sftp_lib();

    /**
     * @brief: 释放sftp库，程序退出时调用一次即可。与init_sftp_lib一一对应
     * @return {*}
     */
    static void deinit_sftp_lib();

private:
    bool init();
    void clear();

private:
    bool is_init_{ false };      // 是否初始化
    std::string host_;           // ftp服务器地址
    std::string user_;           // 用户名
    std::string passwd_;         // 密码

    int socket_fd_{ -1 };        // 套接字文件描述符
    LIBSSH2_SESSION* session_{ nullptr };     // sftp会话
    LIBSSH2_SFTP* sftp_{ nullptr };           // sftp句柄
};


#endif // __SFTP_CLIENT_H__
