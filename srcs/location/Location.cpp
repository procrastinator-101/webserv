#include "Location.hpp"

namespace ft
{
	Location::Location()
	{
	}

	Location::~Location()
	{
	}

	Location::Location(std::ifstream& configFile)
	{
		if (configFile.eof())
			return;
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