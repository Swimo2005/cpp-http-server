#include "Server.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

Server::Server(int p) : port(p), serverSocket(INVALID_SOCKET), isRunning(false) {}

Server::~Server() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}

std::string Server::getFileContent(const std::string& filepath, std::string& contentType) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    // Determine content type based on file extension
    if (filepath.find(".html") != std::string::npos || filepath.find(".htm") != std::string::npos) {
        contentType = "text/html";
    } else if (filepath.find(".css") != std::string::npos) {
        contentType = "text/css";
    } else if (filepath.find(".js") != std::string::npos) {
        contentType = "application/javascript";
    } else {
        contentType = "text/plain";
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Server::start() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsResult != 0) {
        std::cerr << "WSAStartup failed: " << wsResult << "\n";
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        std::cerr << "Bind Failed: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    isRunning = true;
    std::cout << "Server is listening on http://localhost:" << port << "\n";

    while (isRunning) {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &clientSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        char buf[4096];
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived > 0) {
            std::cout << "\n--- Received Request ---\n";
            std::cout.write(buf, bytesReceived);
            std::cout << "------------------------\n";

            std::string httpResponse =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Connection: close\r\n"
                "\r\n"
                "{\"status\": \"success\", \"message\": \"Hello from your custom C++ Code::Blocks server!\"}";

            send(clientSocket, httpResponse.c_str(), (int)httpResponse.size(), 0);
        }

        closesocket(clientSocket);
    }
}
