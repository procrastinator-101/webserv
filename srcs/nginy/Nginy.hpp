#ifndef NGINY_HPP
# define NGINY_HPP

#include <fstream>
#include <string>
#include <vector>

// #include <sys/_types/_fd_def.h>


#include "../client/Client.hpp"
#include "../server/Server.hpp"

#include "../multiplex/Selecter.hpp"

#include "../http_status/HttpStatus.hpp"
#include "../header_field/HeaderField.hpp"

#include "../location/Location.hpp"

#include "../request/Request.hpp"
#include "../response/Response.hpp"

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
			std::map<int, Server *>	_servers;
			std::map<int, Client *>	_clients;
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
			Nginy();
			~Nginy();

			Nginy(const std::string& configFileName);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Nginy operations
		//================================================================================================
		public:
			void	up();
		//================================================================================================
		//	Nginy operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_parseConfigFile();

			void	_parseServerBlock();
			void	_fetchServerSockt(std::stringstream& lineStream);
			void	_fetchServerNames(std::stringstream& lineStream);
			void	_fetchRoot(std::stringstream& lineStream);
			void	_fetchAutoIndex(std::stringstream& lineStream);
			void	_fetchMethods(std::stringstream& lineStream);
			void	_fetchIndexes(std::stringstream& lineStream);
			void	_fetchErrorPages(std::stringstream& lineStream);


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