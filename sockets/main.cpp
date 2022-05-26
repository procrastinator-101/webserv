#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/_types/_socklen_t.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
#include <iomanip>
#include <unistd.h>

//colors
#define BLACK "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define CYAN "\x1B[37m"

#define FIELDSIZE 30

const int bufferSize = 1024;
const int backLog = 10;

struct	Client
{
	int					fd;
	sockaddr_storage	address;
	socklen_t			addressLen;
	char				buffer[bufferSize];
};

int main(int argc, char **argv)
{

	// std::cout << GREEN << "hello" << std::endl;
	int			fd;
	int			ret;
	addrinfo	hints;
	addrinfo	server;
	addrinfo	*result;

	//setting the hints for the listening socket
	bzero(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	// hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo("127.0.0.1", "8080", &hints, &result);
	if (ret || !result)
	{
		std::cout << RED << std::setw(FIELDSIZE) << "address creation" << "failure" << std::endl;
		return 1;
	}
	std::cout << std::left;
	std::cout << GREEN << std::setw(FIELDSIZE) << "address creation" << "success" << std::endl;
	server = *result;
	freeaddrinfo(result);

	//create a socker for listening
	fd = socket(server.ai_family, server.ai_socktype, server.ai_protocol);
	if (fd < 0)
	{
		std::cout << RED << std::setw(FIELDSIZE) << "socket creation" << "failure" << std::endl;
		return 1;
	}
	std::cout << GREEN << std::setw(FIELDSIZE) << "socket creation" << "success" << std::endl;

	//bind the server socket to an addres:port
	ret = bind(fd, server.ai_addr, server.ai_addrlen);
	if (ret)
	{
		std::cout << RED << std::setw(FIELDSIZE) << "socket binding" << "failure" << std::endl;
		close(fd);
		return 1;
	}
	std::cout << GREEN << std::setw(FIELDSIZE) << "socket binding" << "success" << std::endl;
	//listen on the server socket
	ret = listen(fd, backLog);
	if (ret)
	{
		std::cout << RED << std::setw(FIELDSIZE) << "socket listening" << "failure" << std::endl;
		close(fd);
		return 1;
	}
	std::cout << GREEN << std::setw(FIELDSIZE) << "socket listening" << "success" << std::endl;

	//accept connections
	while (1)
	{
		Client client;

		client.addressLen = sizeof(client.address);
		std::cout << YELLOW << std::setw(FIELDSIZE) << "listening" << "..." << std::endl;
		client.fd = accept(fd, reinterpret_cast<sockaddr *>(&client.address), &client.addressLen);
		if (client.fd < 0)
		{
			std::cout << RED << std::setw(FIELDSIZE) << "connections acception" << "failure" << std::endl;
			close(fd);
			return 1;
		}
		std::cout << client.address.ss_family << "|" << client.address.ss_len << std::endl;
		std::cout << server.ai_addr->sa_family << "|" << server.ai_addrlen << std::endl;
		std::cout << AF_INET << "|" << sizeof(sockaddr_storage) << std::endl;
		getnameinfo((struct sockaddr*)&client.address,
               client.addressLen, client.buffer, sizeof(client.buffer), 0, 0,
               NI_NUMERICHOST);
		std::cout << client.buffer << std::endl;
		std::cout << GREEN << "connect with " << client.fd << " : success" << std::endl;
		ret = read(client.fd, client.buffer, bufferSize);
		if (ret < 0)
		{
			std::cout << RED << std::setw(FIELDSIZE) << "reading from client" << "failure" << std::endl;
			close(fd);
			close(client.fd);
			return 1;
		}
		std::cout << GREEN << "reading from " << client.fd << " : success" << std::endl;
		std::string str(client.buffer, ret);
		std::cout << BLACK;
		std::cout << "==============================================================" << std::endl;
		std::cout << str << std::endl;
		std::cout << "==============================================================" << std::endl;
		ret = write(client.fd, client.buffer, ret);
		close(client.fd);
		if (ret < 0)
		{
			std::cout << RED << std::setw(FIELDSIZE) << "writing to client" << "failure" << std::endl;
			close(fd);
			return 1;
		}
		std::cout << GREEN << "writing to " << client.fd << " : success" << std::endl;
	}
	return 0;
}