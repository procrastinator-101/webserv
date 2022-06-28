#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <set>
#include <map>
#include <string>

#include <iomanip>
#include <fstream>
#include <sstream>

#include <cstdlib>

#include "../stdlib/stdlib.hpp"
#include "../http_status/HttpStatus.hpp"

namespace ft
{
	class Location
	{
		friend class Response;
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			std::string							_root;
			bool								_autoIndex;
			std::set<std::string>				_methods;
			std::set<std::string>				_indexes;
			std::pair<int, std::string>			_redirection;
			std::map<std::string, std::string>	_cgis;
			std::string							_uploadPath;
		//================================================================================================
		//	attributes End
		//================================================================================================
		
		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Location();
			~Location();

			//might throw
			Location(std::ifstream& configFile, std::string& root, bool autoIndex, std::set<std::string>& indexes, std::set<std::string>& methods);
			
			Location(const Location& src);
			Location	&operator=(const Location& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Location operations
		//================================================================================================
		public:
			
		//================================================================================================
		//	Location operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Location& src);

			void	_fetchValue(std::stringstream& streamLine, std::string& key);

			void	_fetchRoot(std::stringstream& streamLine);
			void	_fetchMethods(std::stringstream& streamLine);
			void	_fetchIndexes(std::stringstream& streamLine);
			void	_fetchUploadPath(std::stringstream& streamLine);
			void	_fetchAutoIndex(std::stringstream& streamLine);
			void	_fetchRedirections(std::stringstream& streamLine);
			void	_fetchCgi(std::stringstream& streamLine);
		//================================================================================================
		//	private methods End
		//================================================================================================


		//================================================================================================
		//	overload << for Location
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Location& location);
		//================================================================================================
		//	overload << for Location End
		//================================================================================================
	};
}

#endif