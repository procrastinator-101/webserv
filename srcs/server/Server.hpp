#ifndef SERVER_HPP
# define SERVER_HPP

#include <set>
#include <map>
#include <string>
#include <fstream>
#include <utility>


#include "../client/Client.hpp"
#include "../location/Location.hpp"

#include "../sockets/ServerSockt.hpp"

namespace ft
{
	class Server
	{
		friend class Nginy;
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			ServerSockt							_sockt;
			std::set<std::string>				_names;
			std::string							_root;
			bool								_autoIndex;
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
			Server();
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
			
		//================================================================================================
		//	Server operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Server& src);

			void	_fetchSockt(std::stringstream& lineStream);
			void	_fetchServerNames(std::stringstream& lineStream);
			void	_fetchRoot(std::stringstream& lineStream);
			void	_fetchAutoIndex(std::stringstream& lineStream);
			void	_fetchMethods(std::stringstream& lineStream);
			void	_fetchIndexes(std::stringstream& lineStream);
			void	_fetchErrorPages(std::stringstream& lineStream);
			void	_fetchLocation(std::stringstream& lineStream, std::ifstream& configFile);
		//================================================================================================
		//	private methods End
		//================================================================================================
	
		//================================================================================================
		//	overload << for Server
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Server& server);
		//================================================================================================
		//	overload << for Server End
		//================================================================================================
	};
}

#endif