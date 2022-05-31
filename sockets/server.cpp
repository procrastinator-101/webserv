#include "socket.hpp"
#include <exception>
#include <netinet/in.h>
#include <strings.h>
#include <sys/_endian.h>
#include <sys/_types/_in_addr_t.h>
#include <sys/socket.h>

int main(int argc, char **argv)
try
{
	int			ret;
	int			port;
	in_addr_t	address;
	sockaddr_in	scktAddr;

	if (argc != 2)
		throw std::invalid_argument("use : ./server port");
	port = htonl(atoi(argv[2]));
	address = htonl(INADDR_ANY);//!!!!!

	Server server;
	//create a socket
	server.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server.fd < 0)
	{

	}

	//set the address that the socket will bind to
	bzero(&scktAddr, sizeof(sockaddr_in));
	scktAddr.sin_family = AF_INET;
	scktAddr.sin_len = sizeof(sockaddr_in);
	scktAddr.sin_port = port;
	scktAddr.sin_addr.s_addr = address;

	//bind to the address
	ret = bind(server.fd, reinterpret_cast<sockaddr *>(&scktAddr), scktAddr.sin_len);
	if (ret < 0)
	{

	}

	//listen on the socket
	ret = listen(server.fd, server.backlog);
	if (ret < 0)
	{

	}

	//accept connections
	while (1)
	{
		char	buffer[BUFFER_SIZE];
		Client	client;

		client.fd = accept(server.fd, reinterpret_cast<sockaddr *>(&client.address), &client.addressLen);
		if (client.fd < 0)
		{

		}
		ret = recv(client.fd, buffer, BUFFER_SIZE, 0);
		if (ret < 0)
		{

		}
		
	}
}
catch (std::exception& e)
{
	std::cerr << RED;
	std::cerr << e.what() << std::endl;
	std::cerr << BLACK;
	exit(1);
}