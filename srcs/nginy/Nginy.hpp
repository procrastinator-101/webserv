#ifndef NGINY_HPP
# define NGINY_HPP

#include <fstream>
#include <string>
#include <vector>

#include <sys/_types/_fd_def.h>

#include "../sockt/Sockt.hpp"

#include "../client/Client.hpp"
#include "../server/Server.hpp"

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
			std::string	_configFileName;
			std::ifstream	_configFile;
			std::vector<Server>	_servers;
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

			Nginy(const std::string& configFile);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Nginy operations
		//================================================================================================
		public:
			void	select();
		//================================================================================================
		//	Nginy operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_parseConfigFile();
			void	_deepCopy(const Nginy& src);
			fd_set	_getMonitoredSockets() const;
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif