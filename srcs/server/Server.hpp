#ifndef SERVER_HPP
# define SERVER_HPP

#include <set>
#include <map>
#include <string>
#include <fstream>
#include <utility>

#include "../utility/utility.hpp"

#include "../sockt/Sockt.hpp"
#include "../client/Client.hpp"
#include "../location/Location.hpp"

namespace ft
{
	class Server
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			Sockt								_sockt;
			std::set<std::string>				_names;
			std::string							_root;
			status								_autoIndex;
			std::set<std::string>				_methods;
			std::set<std::string>				_indexes;
			std::map<int, std::string>			_errorPages;
			std::map<std::string, Location>		_locations;
			std::map<int, Client>				_clients;
		//================================================================================================
		//	attributes End
		//================================================================================================
		
		
		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Server();//might throw : socket related problem
			~Server();

			//might throw
			Server(std::ifstream& configFile);
			Server(const Server& src);

			Server	&operator=(const Server& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Server operations
		//================================================================================================
		public:
			std::pair<Client*, action>	select();
		//================================================================================================
		//	Server operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Server& src);
			fd_set	_getRawFdSet() const;
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif