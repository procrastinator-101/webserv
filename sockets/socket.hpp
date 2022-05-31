#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

//colors
#define BLACK "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define CYAN "\x1B[37m"

#define FIELDSIZE 30
#define BUFFER_SIZE 1024

struct Server
{
	int	fd;
	int	port;
	int	backlog;
};

struct Client
{
	int			fd;
	sockaddr_in	address;
	socklen_t	addressLen;
};

#define EARG	1

#endif