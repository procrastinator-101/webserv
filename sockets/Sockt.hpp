#ifndef SOCKT_HPP
# define SOCKT_HPP

#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/_types/_socklen_t.h>

#include <ostream>
#include <iomanip>
#include <iostream>

//colors
#define BLACK "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define CYAN "\x1B[37m"

namespace ft
{
	class Sockt
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		public:
			int	fd;
			int	backlog;
			sockaddr_in	address;
		//================================================================================================
		//	attributes End
		//================================================================================================

		//================================================================================================
		//	static attributes
		//================================================================================================
		public:
			const static int		defaultBacklog;
			const static socklen_t	addressLen;
			const static in_port_t	defaultPort;
			const static in_addr_t	defaultAddress;
		//================================================================================================
		//	static attributes End
		//================================================================================================
		
		
		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Sockt& src);
			void	_makeSockt(const in_addr_t& address, const in_port_t& port, const int backlog);
		//================================================================================================
		//	private methods End
		//================================================================================================


		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Sockt();//might throw
			~Sockt();

			Sockt(const Sockt& src);
			Sockt(const int& fd, const int& backlog, const sockaddr_in& address);
			Sockt(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog);//might throw

			Sockt	&operator=(const Sockt& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================

		//================================================================================================
		//	Socket operations
		//================================================================================================
		public:
			void	bind();//might throw
			void	listen();//might throw
			void	accept();//might throw
			void	connect();//might throw
			void	create(const in_addr_t& address, const in_port_t& port, const int backlog);//might throw
		//================================================================================================
		//	Socket operations End
		//================================================================================================


		//================================================================================================
		//	overload << for Sockt
		//================================================================================================
		friend std::ostream	&operator<<(std::ostream& ostr, const ft::Sockt& sockt);
		//================================================================================================
		//	overload << for Sockt End
		//================================================================================================
	};
	
}


#endif