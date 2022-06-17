#include "Host.hpp"

namespace ft
{
			
	Host::Host()
	{
	}

	Host::~Host()
	{
	}

	Host::Host(const Host& src) :	_names(src._names), _root(src._root), _autoIndex(src._autoIndex), _methods(src._methods),
									_indexes(src._indexes), _errorPages(src._errorPages), _locations(src._locations)
	{
	}

	Host	&Host::operator=(const Host& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Host::_deepCopy(const Host& src)
	{
		_names = src._names;
		_root = src._root;
		_autoIndex = src._autoIndex;
		_methods = src._methods;
		_indexes = src._indexes;
		_errorPages = src._errorPages;
		_locations = src._locations;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Host& Host)
	{
		const int	fieldSize = 30;

		ostr << std::left;
		ostr << getDisplayHeader("Host", HOST_HSIZE) << std::endl;

		ostr << getDisplaySubHeader("Host names") << std::endl;
		for (std::set<std::string>::const_iterator it = Host._names.begin(); it != Host._names.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("Host names") << std::endl;

		ostr << std::setw(fieldSize) << "root : " << Host._root << std::endl;
		ostr << std::setw(fieldSize) << "autoIndex : " << Host._autoIndex << std::endl;

		ostr << getDisplaySubHeader("methods") << std::endl;
		for (std::set<std::string>::const_iterator it = Host._methods.begin(); it != Host._methods.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("methods") << std::endl;

		ostr << getDisplaySubHeader("indexes") << std::endl;
		for (std::set<std::string>::const_iterator it = Host._indexes.begin(); it != Host._indexes.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("indexes") << std::endl;

		ostr << getDisplaySubHeader("errorPages") << std::endl;
		for (std::map<int, std::string>::const_iterator it = Host._errorPages.begin(); it != Host._errorPages.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplaySubFooter("errorPages") << std::endl;

		ostr << getDisplaySubHeader("locations") << std::endl;
		for (std::map<std::string, Location>::const_iterator it = Host._locations.begin(); it != Host._locations.end(); ++it)
			ostr << it->second << std::endl;
		ostr << getDisplaySubFooter("locations") << std::endl;

		ostr << getDisplayFooter(HOST_HSIZE) << std::endl;
		return ostr;
	}
}