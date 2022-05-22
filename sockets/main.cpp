#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>

int main(int argc, char **argv)
{
	int	ret;
	int server_fd;
	struct sockaddr_in adress;


	if (argc != 4)
		return 1;
	server_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		std::cerr << "cannot create socket !!!" << std::endl;
		return 1;
	}
	adress.sin_family = AF_INET;
	adress.sin_port = 8080;
	adress.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(server_fd, reinterpret_cast<struct sockaddr *>(&adress), sizeof(sockaddr_in));
	if (ret < 0)
	{
		std::cerr << "cannot bind socket !!!" << std::endl;
		return 1;
	}
	ret = listen(server_fd, 1000);
	if (ret < 0)
	{
		std::cerr << "socket listening failed!!!" << std::endl;
		return 1;
	}

}