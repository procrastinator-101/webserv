#include "Location.hpp"

namespace ft
{
	Location::Location()
	{
	}

	Location::~Location()
	{
	}

	Location::Location(const Location& src) : _index(src._index), _methods(src._methods), _defaultFile(src._defaultFile)
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
		_index = src._index;
		_methods = src._methods;
		_defaultFile = src._defaultFile;
	}
}