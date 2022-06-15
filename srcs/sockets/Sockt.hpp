#ifndef SOCKT_HPP
# define SOCKT_HPP

#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
// #include <sys/_types/_socklen_t.h>

#include <arpa/inet.h>//!!!!!!!!!

#include <ostream>
#include <iomanip>
#include <iostream>

#include <cstring>

#include "../stdlib/stdlib.hpp"

//colors
#define BLACK "\x1B[0m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define CYAN "\x1B[37m"

namespace ft
{

	/**
	 * @brief 
	 * a class to handle sockets
	 * it can be inherited to support server and client sockets
	 */

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
			const static socklen_t	addressLen;
		//================================================================================================
		//	static attributes End
		//================================================================================================
		
		
		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Sockt& src);
			void	_initialise(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog);
		//================================================================================================
		//	private methods End
		//================================================================================================


		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Sockt();
			~Sockt();

			//to support server sockets
			Sockt(const std::string& ipAddress, const std::string& port, const int& bcklog);//might throw
			
			Sockt(const Sockt& src);
			Sockt	&operator=(const Sockt& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================

		//================================================================================================
		//	Socket operations
		//================================================================================================
		public:
			void	open();//might throw
			void	close();

			void	bind();//might throw
			void	listen();//might throw

			Sockt	accept();//might throw

			void	connect(const Sockt dst);//might throw
			void	connect(const in_addr_t& ipAddress, const in_port_t& port);//might throw
		//================================================================================================
		//	Socket operations End
		//================================================================================================


		//================================================================================================
		//	overload << for Sockt
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Sockt& sockt);
		//================================================================================================
		//	overload << for Sockt End
		//================================================================================================
	};
}


#endif