#ifndef HOST_HPP
#define HOST_HPP

#include <set>
#include <map>
#include <string>
#include <fstream>
#include <utility>

#include "../location/Location.hpp"

namespace ft
{
	class Host
	{
		friend class Nginy;
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			std::set<std::string>				_names;
			std::string							_root;
			bool								_autoIndex;
			std::set<std::string>				_methods;
			std::set<std::string>				_indexes;
			std::map<int, std::string>			_errorPages;
			std::map<std::string, Location>		_locations;
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