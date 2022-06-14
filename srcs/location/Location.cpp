#include "Location.hpp"

namespace ft
{
	Location::Location()
	{
	}

	Location::~Location()
	{
	}

	Location::Location(std::ifstream& configFile) : _root(NULL), _autoIndex(off), _methods(), _indexes(), _redirections(), _uploadPath(NULL)
	{
		int				inside_location = 0;
		std::string		line;

		std::getline(configFile, line);
		std::stringstream	streamLine(line);
		std::string			token;
		streamLine >> token;
		if (token != "{")
			throw std::runtime_error("Location: config file is not valid");
		
		while (std::getline(configFile, line))
		{
			inside_location = 1;
			if (line.empty())
				continue ;
			streamLine.clear();
			streamLine.str(line);
			streamLine >> token;
			if (token == "#")
				continue ;
			if (token == "}")
			{
				inside_location = 0;
				break ;
			}

			
		}
		if (inside_location == 1)
			throw std::runtime_error("Location: config file is not valid");
	}

	Location::Location(const Location& src) :	_root(src._root), _autoIndex(src._autoIndex), _methods(src._methods), _indexes(src._indexes),
												_redirections(src._redirections), _uploadPath(src._uploadPath)
	{
	}

	Location	&Location::operator=(const Location& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Location::_deepCopy(const Location& src)
	{
		_root = src._root;
		_autoIndex = src._autoIndex;
		_methods = src._methods;
		_indexes = src._indexes;
		_redirections = src._redirections;
		_uploadPath = src._uploadPath;
	}
}