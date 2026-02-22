#include <iostream>
#include "HttpServer.h"

int main() {
    HttpServer server;
    server.start(8080);
    return 0;
}
