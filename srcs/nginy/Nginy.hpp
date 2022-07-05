#ifndef NGINY_HPP
# define NGINY_HPP

#include <fstream>
#include <string>
#include <vector>


#include "../client/Client.hpp"
#include "../server/Server.hpp"

#include "../multiplex/Selecter.hpp"

#include "../http_status/HttpStatus.hpp"

#include "../location/Location.hpp"

#include "../request/Request.hpp"
#include "../response/Response.hpp"

# define MULTIPLEXING_TIMEOUT	2000	//milliseconds

namespace ft
{
	class Nginy
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			std::string				_configFileName;
			Selecter				_multiplexer;
			std::vector<Server *>	_servers;
		//================================================================================================
		//	attributes End
		//================================================================================================


		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		private:
			Nginy(const Nginy& src);
			Nginy	&operator=(const Nginy& rop);

		public:
			~Nginy();

			Nginy();
			Nginy(const std::string& configFileName);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Nginy operations
		//================================================================================================
		public:
			void	up();
			void	fetchConfiguration(const std::string& configFileName);//to remove
		//================================================================================================
		//	Nginy operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_manageRequest(Client& client, Server& server);
			void	_manageResponse(Client& client, Server& server);

			void	_manageTimeouts();
			void	_parseConfigFile();

			//server block parsing functions
			void	_parseServerBlock(std::ifstream& configFile);
			ServerSockt	_fetchServerSockt(std::stringstream& lineStream);
			void	_fetchHostValue(Host& host, std::ifstream& configFile, std::stringstream &lineStream, std::string& key);

			void	_addHost(ServerSockt& sockt, Host& host);

			//miscellaneous private methods
			void	_deepCopy(const Nginy& src);
			void	_initiateServers();
			void	_serveClients(std::map<int, int>& candidates);
			void	_acceptNewClients(std::map<int, int>& candidates);
		//================================================================================================
		//	private methods End
		//================================================================================================

		//================================================================================================
		//	overload << for Nginy
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Nginy& nginy);
		//================================================================================================
		//	overload << for Nginy End
		//================================================================================================
	
	};
}

#endif