#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include <LLauncher.h>


class IPCServer {
public:
    IPCServer() {
        unlink(m_socket_path); // Remove old socket if it exists
    }
bool start() noexcept;
void executeCommand(const std::string &command) noexcept;
void handleClients() noexcept;
void processClient(int clientFd) noexcept;

~IPCServer() {
    if (m_sockfd != -1)
        close(m_sockfd);

    unlink(m_socket_path);
}

private:
    int m_sockfd = -1;
    const char* m_socket_path = "/tmp/nebula_ipc.sock";
};
