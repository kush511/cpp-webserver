#pragma once
#include <string>
#include <vector>
#include <winsock2.h>

class HttpTcpServer {
public:
    HttpTcpServer(int port);
    ~HttpTcpServer();
    void start();

private:
    SOCKET serverSocket;
    void init();
    void acceptConnections();
    std::string buildHttpResponse(const std::string& request);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::string getMimeType(const std::string& path);
    std::vector<char> readBinaryFile(const std::string& path);
    std::string readTextFile(const std::string& path);
};
