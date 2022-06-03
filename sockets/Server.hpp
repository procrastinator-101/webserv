#ifndef SERVER_HPP
# define SERVER_HPP

#include <set>
#include <map>
#include <string>

#include "Sockt.hpp"
#include "Client.hpp"
#include "Location.hpp"

namespace ft
{
	class Server
	{
		private:
			Sockt	_sockt;
			std::string		_name;
			std::string		_root;
			std::set<std::string>	_aliases;
			std::map<std::string, Location>	_locations;
			std::map<int, Client>			_clients;
		
			void	_deepCopy(const Server& src);

		public:
			//might throw : socket related problem
			Server();
			~Server();

			Server(const Server& src);

			Server	&operator=(const Server& rop);
	};
}

#endif