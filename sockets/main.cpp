#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <unistd.h>

typedef struct sockaddr t_sockaddr;
typedef struct sockaddr_in t_sockaddr_in;

const int bufferSize = 1024;

struct sckt
{
	int				fd;
	char			buffer[bufferSize];
	socklen_t		addressLen;
	t_sockaddr_in	adress;
};

int main()
{
	int		ret;
	sckt	cnctSocket;
	sckt	mainSocket;

	mainSocket.fd  = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket.fd < 0)
	{
		std::cerr << "cannot create socket !!!" << std::endl;
		return 1;
	}
	mainSocket.adress.sin_family = AF_INET;
	mainSocket.adress.sin_port = 8080;
	mainSocket.adress.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(mainSocket.fd, reinterpret_cast<t_sockaddr *>(&mainSocket.adress), sizeof(t_sockaddr_in));
	if (ret < 0)
	{
		std::cerr << "cannot bind socket !!!" << std::endl;
		return 1;
	}
	ret = listen(mainSocket.fd, 1000);
	if (ret < 0)
	{
		std::cerr << "socket listening failed!!!" << std::endl;
		return 1;
	}
	cnctSocket.fd = accept(mainSocket.fd, reinterpret_cast<t_sockaddr *>(&cnctSocket.adress), &cnctSocket.addressLen);
	if (cnctSocket.fd < 0)
	{
		std::cerr << "accept failed!!!" << std::endl;
		close(mainSocket.fd);
		return 1;
	}
	ret = read(cnctSocket.fd, cnctSocket.buffer, bufferSize);
	close(mainSocket.fd);
	close(cnctSocket.fd);
	if (ret < 0)
	{
		std::cerr << "read failed!!!" << std::endl;
		return 1;
	}
	write(1, cnctSocket.buffer, ret);
	std::cout << "server is shut down" << std::endl;
	return 0;
}