#include "server.h"

int main() {
	try {
		Server server("192.168.0.15", 63486);
		if (server.start()) {
			server.run();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Server error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}