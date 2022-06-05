#ifndef SERVER_HPP
# define SERVER_HPP

#include <set>
#include <map>
#include <string>

#include "../sockt/Sockt.hpp"
#include "../client/Client.hpp"
#include "../location/Location.hpp"

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
			Server();//might throw : socket related problem
			~Server();

			Server(const Server& src);

			Server	&operator=(const Server& rop);
	};
}

#endif