#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <winsock2.h>
#include <string>

class HttpServer {
public:
    void start(int port);
private:
    void handleClient(SOCKET clientSocket);
    std::string parseJsonValue(const std::string& json, const std::string& key);
    void sendResponse(SOCKET clientSocket, const std::string& body, const std::string& contentType = "application/json");
};

#endif
