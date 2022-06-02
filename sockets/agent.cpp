#include "Sockt.hpp"
#include <exception>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>

int main(int argc, char **argv)
{
	int			ret;
	sockaddr_in	dest;
	addrinfo	hints;
	addrinfo	*reslt;
	ft::Sockt	client;
	std::string	message;

	if (argc != 3)
	{
		std::cerr << "use : ./client host port" << std::endl;
		return 1;
	}
	try
	{
		//setting the hints for the connecting socket
		bzero(&hints, sizeof(addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_addrlen = sizeof(sockaddr_in);
		ret = getaddrinfo(argv[1], argv[2], &hints, &reslt);
		if (ret < 0 || !reslt)
			throw std::runtime_error("addrinfo failure");
		dest = *((sockaddr_in *)&reslt->ai_addr);
		freeaddrinfo(reslt);
		client.makeClientSockt(dest.sin_addr.s_addr, dest.sin_port);
		std::cin >> message;
		ret = send(client.fd, message.c_str(), message.length(), 0);
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