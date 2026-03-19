#pragma once
#include <string>
#include <functional>

class SocketServer {
public:
    explicit SocketServer(int port);
    ~SocketServer();

    // Starts listening, calls handler for each message received
    void start(std::function<std::string(const std::string&)> handler);
    void stop();

private:
    int port_;
    int server_fd_ = -1;
    bool running_  = false;
};
