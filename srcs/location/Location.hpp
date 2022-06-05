#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <set>
#include <string>

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
			status					_index;
			std::set<std::string>	_methods;
			std::string				_defaultFile;
			//redirections

			void	_deepCopy(const Location& src);
		
		public:
			Location();
			~Location();

			Location(const Location& src);
			
			Location	&operator=(const Location& rop);
	};
}

#endif