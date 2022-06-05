#include "Sockt.hpp"
#include <arpa/inet.h>
#include <exception>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>

int main(int argc, char **argv)
{
	int			ret;
	ft::Sockt	client;
	std::string	message;

	if (argc != 3)
	{
		std::cerr << "use : ./client host port" << std::endl;
		return 1;
	}
	try
	{
		client.makeClientSockt(inet_addr(argv[1]), htons(atoi(argv[2])));
		std::cin >> message;
		ret = send(client.fd, message.c_str(), message.length(), 0);
		if (ret < 0)
			throw std::runtime_error("send failure");
		client.destroy();
	}
	catch (std::exception& e)
	{
		client.destroy();
		std::cerr << RED;
		std::cerr << e.what() << std::endl;
		std::cerr << BLACK;
		exit(EXIT_FAILURE);
	}
	return 0;
}