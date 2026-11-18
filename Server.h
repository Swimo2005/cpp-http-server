#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class Server {
private:
    int port;
    SOCKET serverSocket;
    bool isRunning;

    // Helper method to read files from disk and determine content type
    std::string getFileContent(const std::string& filepath, std::string& contentType);

public:
    Server(int port);
    ~Server();
    void start();
};

#endif // SERVER_H
