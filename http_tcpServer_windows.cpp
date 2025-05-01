#include "http_tcpServer_windows.h"
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

HttpTcpServer::HttpTcpServer(int port) {
    init();

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is running on port " << port << "...\n";
}

HttpTcpServer::~HttpTcpServer() {
    closesocket(serverSocket);
    WSACleanup();
}

void HttpTcpServer::init() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << "\n";
        exit(EXIT_FAILURE);
    }
}

void HttpTcpServer::start() {
    acceptConnections();
}

void HttpTcpServer::acceptConnections() {
    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientSize = sizeof(clientAddr);

    while ((clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize)) != INVALID_SOCKET) {
        char buffer[8192] = {0}; // increased buffer size
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            std::string request(buffer, bytesReceived);
            std::cout << "\nReceived Request:\n" << request << "\n";

            std::string response = buildHttpResponse(request);
            send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);
        }

        closesocket(clientSocket);
    }
}

std::string HttpTcpServer::buildHttpResponse(const std::string& request) {
    std::istringstream requestStream(request);
    std::string method, path;
    requestStream >> method >> path;
    if (path == "/") {
        path = "/index.html";
    } else if (path.find('.') == std::string::npos) {
        path += ".html";
    }
    
    
    std::string fullPath = "www" + path;
    std::string contentType = getMimeType(fullPath);

    if (contentType.find("image") != std::string::npos) {
        std::vector<char> imageData = readBinaryFile(fullPath);
        if (imageData.empty()) return "HTTP/1.1 404 Not Found\r\n\r\n";

        std::ostringstream header;
        header << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: " << contentType << "\r\n"
               << "Content-Length: " << imageData.size() << "\r\n"
               << "Connection: close\r\n\r\n";

        std::string response = header.str();
        response.append(imageData.begin(), imageData.end());
        return response;
    } else {
        std::string html = readTextFile(fullPath);
        if (html.empty()) return "HTTP/1.1 404 Not Found\r\n\r\n";

        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << contentType << "\r\n"
                 << "Content-Length: " << html.size() << "\r\n"
                 << "Connection: close\r\n\r\n"
                 << html;

        std::cout << "Requested Path: " << path << "\n";
        std::cout << "Full Path: " << fullPath << "\n";

        return response.str();
    }
}

bool HttpTcpServer::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string HttpTcpServer::getMimeType(const std::string& path) {
    if (endsWith(path, ".html")) return "text/html";
    if (endsWith(path, ".css")) return "text/css";
    if (endsWith(path, ".js")) return "application/javascript";
    if (endsWith(path, ".png")) return "image/png";
    if (endsWith(path, ".jpg") || endsWith(path, ".jpeg")) return "image/jpeg";
    if (endsWith(path, ".gif")) return "image/gif";
    return "application/octet-stream";
}

std::vector<char> HttpTcpServer::readBinaryFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    return std::vector<char>((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
}

std::string HttpTcpServer::readTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";
    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}
