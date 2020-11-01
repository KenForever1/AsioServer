#include <iostream>
#include "Server.hpp"

void TestServer(){
    io_service ios;

    Server server(ios,9900);
    server.Accept();

    ios.run();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    TestServer();
    return 0;
}
