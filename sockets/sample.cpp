#include <cstdio>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <unistd.h>

const short int port = 8080;
const int bufferSize = 1024;

int main()
{
	int	ret;
	int server, client;
	sockaddr_in	address;
	socklen_t	len = sizeof(address);
	char buffer[bufferSize];

	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0)
	{
		std::cerr << "socket creation failed" << std::endl;
		return 1;
	}
	std::cout << "socket creation : success , server :" << server << std::endl;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	ret = bind(server, (sockaddr *)&address, sizeof(address));
	if (ret < 0)
	{
		std::cerr << "socket binding failed" << std::endl;
		close(server);
		return 1;
	}
	std::cout << "socket binding : success" << std::endl;
	ret = listen(server, 10);
	if (ret < 0)
	{
		std::cerr << "socket listening failed" << std::endl;
		close(server);
		return 1;
	}
	std::cout << "listening ....." << std::endl;
	client = accept(server, (sockaddr *)&address, &len);
	if (client < 0)
	{
		std::cerr << "accept failed" << std::endl;
		close(server);
		return 1;
	}
	std::cout << "connection accepted" << std::endl;
	std::cout << address.sin_family << " " << len << std::endl;
	ret = read(client, buffer, bufferSize);
	close(server);
	close(client);
	if (ret < 0)
	{
		std::cerr << "read failed!!!" << std::endl;
		return 1;
	}
	write(1, buffer, ret);
	std::cout << "server is shut down" << std::endl;
	return 0;
}