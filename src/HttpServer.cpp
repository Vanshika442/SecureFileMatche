#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Matcher.h"
#include "Crypto.h"
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

#include "HttpServer.h"
#include "Matcher.h"
#include "Crypto.h"


void HttpServer::start(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Server listening on port " << port << "..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        
        // Handle in main thread for simplicity in this demo, or use std::thread
        handleClient(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
}

void HttpServer::handleClient(SOCKET clientSocket) {
    // 1. Read Headers
    std::string request;
    char buffer[4096];
    int headerEnd = -1;

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            return;
        }
        request.append(buffer, bytesReceived);
        
        headerEnd = request.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            break;
        }
    }

    // 2. Parse Content-Length
    size_t bodyStart = headerEnd + 4;
    size_t contentLength = 0;
    std::string lowerRequest = request; 
    // Basic formatting for case-insensitive search (manual for simplicity)
    for(auto& c : lowerRequest) c = tolower(c);
    
    size_t clPos = lowerRequest.find("content-length:");
    if (clPos != std::string::npos) {
        size_t sol = lowerRequest.find("\r\n", clPos);
        std::string clStr = request.substr(clPos + 15, sol - (clPos + 15));
        try {
            contentLength = std::stoull(clStr);
        } catch (...) {
            contentLength = 0;
        }
    }

    // 3. Read Body Loop
    size_t currentBodySize = request.size() - bodyStart;
    
    // Safety check: Don't allow infinite growth, cap at 10MB for this demo
    if (contentLength > 10 * 1024 * 1024) { 
        std::string resp = "HTTP/1.1 413 Payload Too Large\r\n\r\n";
        send(clientSocket, resp.c_str(), resp.size(), 0);
        closesocket(clientSocket);
        return;
    }

    // Allocate buffer for rest of body to avoid string reallocations if possible, 
    // but appending to string is easier for logic here.
    while (currentBodySize < contentLength) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break;
        }
        request.append(buffer, bytesReceived);
        currentBodySize += bytesReceived;
    }
    
    // Parse Method
    std::string method;
    std::stringstream ss(request);
    ss >> method;

    // Handle CORS preflight
    if (method == "OPTIONS") {
        std::string response = 
            "HTTP/1.1 204 No Content\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
        closesocket(clientSocket);
        return;
    }

    if (method == "POST") {
        std::string body = request.substr(bodyStart);

        // Parse JSON
        std::string text = parseJsonValue(body, "text");
        std::string pattern = parseJsonValue(body, "pattern");
        std::string algorithm = parseJsonValue(body, "algorithm");
        std::string keyStr = parseJsonValue(body, "key");
        
        int key = 1; // Default key
        if (!keyStr.empty()) {
            try {
                key = std::stoi(keyStr);
            } catch (...) {
                key = 1;
            }
        }

        // Logic
        std::string decryptedText = Crypto::decrypt(text, key);
        
        auto matcher = MatcherFactory::create(algorithm);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        int index = matcher->search(decryptedText, pattern);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        long long duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        // Response
        std::string responseBody = "{";
        responseBody += "\"found\": " + std::string(index != -1 ? "true" : "false") + ",";
        responseBody += "\"index\": " + std::to_string(index) + ",";
        responseBody += "\"time_ms\": " + std::to_string(duration / 1000.0); // Convert to ms
        responseBody += "}";

        sendResponse(clientSocket, responseBody);
    } else {
        std::string resp = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientSocket, resp.c_str(), resp.size(), 0);
    }

    closesocket(clientSocket);
}

// Very basic manual JSON parser for demonstration
std::string HttpServer::parseJsonValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\":";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t startVal = json.find("\"", keyPos + searchKey.length());
    if (startVal == std::string::npos) return "";
    startVal++; // skip quote

    size_t endVal = startVal;
    while (endVal < json.length()) {
        if (json[endVal] == '"' && json[endVal-1] != '\\') {
            break;
        }
        endVal++;
    }

    return json.substr(startVal, endVal - startVal);
}

void HttpServer::sendResponse(SOCKET clientSocket, const std::string& body, const std::string& contentType) {
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;
    send(clientSocket, response.c_str(), response.size(), 0);
}
