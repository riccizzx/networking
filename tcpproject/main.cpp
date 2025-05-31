#include "server.h"

int main() {
	try {
		Server server("", 0);
		if (server.start()) {
			server.run();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Server error: " << e.what() << "\n";
		return 1;
	}

	WSACleanup();
	return 0;

	
}