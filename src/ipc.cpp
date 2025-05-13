#include "ipc.hpp"
#include <sys/socket.h>

bool IPCServer::start() noexcept {
    m_sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_sockfd == -1) {
        perror("Socket creation failed");
        return false;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, m_socket_path, sizeof(addr.sun_path) - 1);

    if (bind(m_sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Bind failed");
        return false;
    }

    if (listen(m_sockfd, 5) == -1) {
        perror("Listen failed");
        return false;
    }

    fcntl(m_sockfd, F_SETFL, O_NONBLOCK); // Make it non-blocking

    std::cout << "IPC Server started on " << m_socket_path << std::endl;
    return true;
}

void IPCServer::handleClients() noexcept {
    int clientFd = accept(m_sockfd, nullptr, nullptr);
    if (clientFd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("Accept failed");
        }
    }

    std::thread(&IPCServer::processClient, this, clientFd).detach();
}

void IPCServer::processClient(int clientFd) noexcept {
    char buffer[256] = {0};
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::string command(buffer);
        std::cout << "Received command: " << command << std::endl;
        executeCommand(command);
    }

    close(clientFd);
}

void IPCServer::executeCommand(const std::string &command) noexcept {

    if (command == "alacritty") {
        LLauncher::launch("alacritty");
    }
}
