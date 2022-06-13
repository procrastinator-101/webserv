#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <set>
#include <map>
#include <string>
#include <fstream>

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
			std::map<int, std::string>	_redirections;
			std::string					_uploadPath;

			void	_deepCopy(const Location& src);
		
		public:
			Location();
			~Location();

			//might throw
			Location(std::ifstream& configFile);
			Location(const Location& src);
			
			Location	&operator=(const Location& rop);
	};
}

#endif