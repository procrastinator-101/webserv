#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <set>
#include <map>
#include <string>
#include <fstream>

#include "../http_status/HttpStatus.hpp"

namespace ft
{
	enum status
	{
		on = true,
		off = false
	};

	class Location
	{
		private:
			std::string					_root;
			status						_autoIndex;
			std::set<std::string>		_methods;
			std::set<std::string>		_indexes;
			std::pair<int, std::string>	_redirection;
			std::string					_uploadPath;

			void	_deepCopy(const Location& src);

			void	threat_line(std::stringstream& streamLine, std::string& token);
			void	threat_root(std::stringstream& streamLine, std::string& token);
			void	threat_methods(std::stringstream& streamLine, std::string& token);
			void	threat_indexes(std::stringstream& streamLine, std::string& token);
			void	threat_upload(std::stringstream& streamLine, std::string& token);
			void	threat_autoindex(std::stringstream& streamLine, std::string& token);
			void	threat_return(std::stringstream& streamLine, std::string& token);
		
		public:
			Location();
			~Location();

			//might throw
			Location(std::ifstream& configFile, std::string& root, status autoIndex, std::set<std::string>& indexes, std::set<std::string>& methods);
			Location(const Location& src);
			
			Location	&operator=(const Location& rop);
	};
}

#endif