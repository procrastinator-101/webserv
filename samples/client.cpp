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

struct	Server
{
	int					fd;
	sockaddr_storage	address;
	socklen_t			addressLen;
	char				buffer[bufferSize];
};

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "use : ./client host port" << std::endl;
		return 1;
	}
	
	int			fd;
	int			ret;
	addrinfo	host;
	addrinfo	hints;
	addrinfo	*result;

	//setting the hints for the connecting socket
	bzero(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_addrlen = sizeof(sockaddr_in);

	ret = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (ret || !result)
	{
		std::cout << RED << std::setw(FIELDSIZE) << "address creation" << "failure" << BLACK << std::endl;
		return 1;
	}
	std::cout << std::left;
	std::cout << GREEN << std::setw(FIELDSIZE) << "address creation" << "success" << BLACK << std::endl;
	host = *result;
	freeaddrinfo(result);

	while (1)
	{
		//create a socker to connect to a server
		fd = socket(host.ai_family, host.ai_socktype, host.ai_protocol);
		if (fd < 0)
		{
			std::cout << RED << std::setw(FIELDSIZE) << "socket creation" << "failure" << BLACK << std::endl;
			return 1;
		}
		std::cout << GREEN << std::setw(FIELDSIZE) << "socket creation" << "success" << BLACK << std::endl;

		ret = connect(fd, host.ai_addr, host.ai_addrlen);
		if (ret < 0)
		{
			std::cout << RED << std::setw(FIELDSIZE) << "connect" << "failure" << BLACK << std::endl;
			close(fd);
		}
		std::cout << GREEN << std::setw(FIELDSIZE) << "connect" << "success" << BLACK << std::endl;
		std::string msg;
		std::cin >> msg;
		ret = send(fd, msg.c_str(), msg.length(), 0);
		if (ret < 0 || ret != msg.length())
		{
			std::cout << RED << std::setw(FIELDSIZE) << "send : " << ret << " failure" << BLACK << std::endl;
			close(fd);
		}
		std::cout << GREEN << std::setw(FIELDSIZE) << "send" << "success" << BLACK << std::endl;
		close(fd);
	}
	return 0;
}