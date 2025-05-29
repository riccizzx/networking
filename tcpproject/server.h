#define SERVER_H

#ifdef SERVER_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

class Server {
public:
    Server(const std::string& ip, int port);
    ~Server();

    bool start();
    void stop();
    void run();

private:
    void initializeWinsock();
    void createSocket();

    std::string ip_;
    int port_;
    SOCKET serverSocket_;
    WSADATA wsaData_;
};

#endif // SERVER_H