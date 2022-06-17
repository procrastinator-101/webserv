#include "Location.hpp"

namespace ft
{
	Location::Location()
	{
	}

	Location::~Location()
	{
	}

	Location::Location(const Location& src) :	_root(src._root), _autoIndex(src._autoIndex), _methods(src._methods), _indexes(src._indexes),
												_redirection(src._redirection), _uploadPath(src._uploadPath)
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
		_redirection = src._redirection;
		_uploadPath = src._uploadPath;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Location& location)
	{
		const int	fieldSize = 30;

		ostr << std::left;
		ostr << getDisplayHeader("Location", LOCATION_HSIZE) << std::endl;


		ostr << std::setw(fieldSize) << "root : " << location._root << std::endl;
		ostr << std::setw(fieldSize) << "autoIndex : " << location._autoIndex << std::endl;

		ostr << getDisplaySubHeader("methods") << std::endl;
		for (std::set<std::string>::const_iterator it = location._methods.begin(); it != location._methods.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("methods") << std::endl;

		ostr << getDisplaySubHeader("indexes") << std::endl;
		for (std::set<std::string>::const_iterator it = location._indexes.begin(); it != location._indexes.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("indexes") << std::endl;

		ostr << getDisplaySubHeader("redirection") << std::endl;
		ostr << std::setw(fieldSize) << location._redirection.first << " : " << location._redirection.second << std::endl;
		ostr << getDisplaySubFooter("redirection") << std::endl;

		ostr << std::setw(fieldSize) << "uploadPath : " << location._uploadPath << std::endl;

		ostr << getDisplayFooter(LOCATION_HSIZE) << std::endl;
		return ostr;
	}
}