#ifndef HOST_HPP
#define HOST_HPP

#include <set>
#include <map>
#include <string>
#include <utility>

#include <iomanip>
#include <fstream>

#include "../location/Location.hpp"

# define DEFAULT_MAX_BODY_SIZE	1048576

namespace ft
{
	class Host
	{
		friend class Nginy;
		friend class Response;
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			std::set<std::string>				_names;
			std::string							_root;
			bool								_autoIndex;
			size_t								_maxBodySize;
			std::set<std::string>				_methods;
			std::set<std::string>				_indexes;
			std::map<int, std::string>			_errorPages;
			std::map<std::string, Location *>	_locations;
		//================================================================================================
		//	attributes End
		//================================================================================================
		
		
		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Host();
			~Host();

			Host(const Host& src);

			Host	&operator=(const Host& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Host operations
		//================================================================================================
		public:
			bool	hasName(const std::string& name) const;
			void	fetchNames(std::stringstream& lineStream);
			void	fetchRoot(std::stringstream& lineStream);
			void	fetchAutoIndex(std::stringstream& lineStream);
			void	fetchMethods(std::stringstream& lineStream);
			void	fetchIndexes(std::stringstream& lineStream);
			void	fetchErrorPages(std::stringstream& lineStream);
			void	fetchBodySize(std::stringstream& lineStream);
			void	fetchLocation(std::ifstream& configFile, std::stringstream& lineStream);
		//================================================================================================
		//	Host operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Host& src);
		//================================================================================================
		//	private methods End
		//================================================================================================
	
		//================================================================================================
		//	overload << for Host
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Host& host);
		//================================================================================================
		//	overload << for Host End
		//================================================================================================
	};
}

#endif