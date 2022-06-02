#include "Sockt.hpp"
#include <exception>
#include <stdexcept>

int main()
{
	int			ret;
	char		buffer[1024];
	ft::Sockt	client;
	ft::Sockt	server;

	try
	{
		server.create();
		server.bind();
		server.listen();
		std::cout << server << std::endl;
		std::cout << "listening ..." << std::endl;
		client = server.accept();
		std::cout << "pending ..." << std::endl;

		ret = recv(client.fd, buffer, 1024, 0);
		if (ret < 0)
			throw std::runtime_error("recv failure");
		ret = send(client.fd, buffer, ret, 0);
		if (ret < 0)
			throw std::runtime_error("send failure");
	}
	catch (std::exception& e)
	{
		std::cerr << RED;
		std::cerr << e.what() << std::endl;
		std::cerr << BLACK;
		exit(EXIT_FAILURE);
	}
	return 0;
}