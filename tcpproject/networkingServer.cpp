#include "server.h"

//
Server::Server(const std::string& ip, int port)
    : ip_(ip), port_(port), serverSocket_(INVALID_SOCKET) {
    try {
        initializeWinsock();
        createSocket();
    } catch (const std::exception& e) {
        std::cerr << "Server initialization error: " << e.what() << "\n";
        throw;
    }
}

// destructor to clean up resources
Server::~Server() {
    stop();
    WSACleanup();
}

// initialize Winsock
void Server::initializeWinsock() {
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData_);
    if (result != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result));
    }
}

//socket config, TCP: SOCK_STREAM, IPv4: AF_INET, initialize in 0;
void Server::createSocket() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == INVALID_SOCKET) {
        int err = WSAGetLastError();
        WSACleanup();
        throw std::runtime_error("Socket creation failed. Error: " + std::to_string(err));
    }
}

//start server recive IP and PORT 
bool Server::start() {
    
    std::cout << "PORT: ";
    std::cin >> port_;
	std::cout << "IP: ";
	std::cin >> ip_;
    
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port_);
	inet_pton(AF_INET, ip_.c_str(), &serverAddr.sin_addr);

	// initialize the server address structure
	// bind the socket to the specified IP address and port
	if (bind(serverSocket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		closesocket(serverSocket_);
		WSACleanup();
		throw std::runtime_error("Bind failed. Error: " + std::to_string(err));
	}

	// Start listening for incoming connections
	if (listen(serverSocket_, SOMAXCONN) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		closesocket(serverSocket_);
		WSACleanup();
		throw std::runtime_error("Listen failed. Error: " + std::to_string(err));
	}
	return true;
}

// Run the server to accept incoming connections
void Server::run() {
    std::cout << "Server is running on " << ip_ << ":" << port_ << "\n";

    while (true) {
        sockaddr_in client;
        int clientSize = sizeof(client);

        SOCKET clientSocket = accept(serverSocket_, (sockaddr*)&client, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed. Error: " << WSAGetLastError() << "\n";
            continue;
        }

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        std::memset(host, 0, NI_MAXHOST);
        std::memset(service, 0, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            std::cout << host << " connected on port " << service << "\n";
        }
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connected on port " << ntohs(client.sin_port) << "\n";
        }

        // Communication loop (echo server)
        const int bufferSize = 4096;
        char buff[bufferSize];

        while (true) {
            std::memset(buff, 0, bufferSize);

            int bytesReceived = recv(clientSocket, buff, bufferSize, 0);
            if (bytesReceived == SOCKET_ERROR) {
                std::cerr << "recv() failed. Error: " << WSAGetLastError() << "\n";
                break;
            }

            if (bytesReceived == 0) {
                std::cout << "Client disconnected.\n";
                break;
            }

            int bytesSent = send(clientSocket, buff, bytesReceived, 0);
			if (bytesSent == SOCKET_ERROR) {
				std::cerr << "send() failed. Error: " << WSAGetLastError() << "\n";
				break;
            }
        }

        closesocket(clientSocket);
        std::cout << "Connection closed.\n";
    }
}

// Stop the server and clean up resources
void Server::stop() {
	if (serverSocket_ != INVALID_SOCKET) {
		closesocket(serverSocket_);
		serverSocket_ = INVALID_SOCKET;
	}
	WSACleanup();
    std::cout << "Server stopped." << "\n";
}