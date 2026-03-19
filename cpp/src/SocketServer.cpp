#include "SocketServer.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

SocketServer::SocketServer(int port) : port_(port) {}

SocketServer::~SocketServer() { stop(); }

void SocketServer::stop() {
    running_ = false;
    if (server_fd_ >= 0) { close(server_fd_); server_fd_ = -1; }
}

void SocketServer::start(std::function<std::string(const std::string&)> handler) {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) throw std::runtime_error("socket() failed");

    // Allow port reuse immediately after restart
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    listen(server_fd_, 5);
    running_ = true;
    std::cout << "[Server] Listening on port " << port_ << std::endl;

    while (running_) {
        sockaddr_in client{};
        socklen_t len = sizeof(client);
        int client_fd = accept(server_fd_, (sockaddr*)&client, &len);
        if (client_fd < 0) break;

        std::cout << "[Server] Client connected" << std::endl;

        // Read request (newline-terminated)
        std::string request;
        char buf[4096];
        while (true) {
            int n = recv(client_fd, buf, sizeof(buf) - 1, 0);
            if (n <= 0) break;
            buf[n] = '\0';
            request += buf;
            if (request.find('\n') != std::string::npos) break;
        }

        // Process and respond
        std::string response = handler(request);
        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
        std::cout << "[Server] Response sent, connection closed" << std::endl;
    }
}
