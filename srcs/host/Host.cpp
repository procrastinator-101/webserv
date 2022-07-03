#include "Host.hpp"

namespace ft
{
			
	Host::Host() : _maxBodySize(DEFAULT_MAX_BODY_SIZE)
	{
	}

	Host::~Host()
	{
		for (std::map<std::string, Location *>::iterator it = _locations.begin(); it != _locations.end(); ++it)
			delete it->second;
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

	bool	Host::hasName(const std::string& name) const
	{
		std::set<std::string>::const_iterator	it;

		it = _names.find(name);
		return it != _names.end();
	}

	void	Host::fetchNames(std::stringstream& lineStream)
	{
		size_t	size;
		std::string	value;

		size = _names.size();
		while (lineStream.good())
		{
			value.clear();
			lineStream >> value;
			if (!value.length() || value == "#")
				break ;
			_names.insert(value);
		}
		if (_names.size() == size)
			throw std::runtime_error("Host:: server_name is not valid");
	}

	void	Host::fetchRoot(std::stringstream& lineStream)
	{
		std::string	value;
		std::string	check;

		lineStream >> value;
		if (value != "#")
			_root = value;
		else
			throw std::runtime_error("Host:: root is not valid");
		if (!is_path_valid(_root))
			throw std::runtime_error("Host:: root is not valid");
		if (lineStream.good())
		{
			lineStream >> check;
			if (check.length() && check != "#")
				throw std::runtime_error("Host:: too many arguments for root");
		}
	}

	void	Host::fetchAutoIndex(std::stringstream& lineStream)
	{
		std::string	value;

		lineStream >> value;
		if (value == "on")
			_autoIndex = true;
		else if (value != "off")
			throw std::runtime_error("Host:: autoindex is not valid");
		if (lineStream.good())
		{
			value.clear();
			lineStream >> value;
			if (value.length() && value != "#")
				throw std::runtime_error("Host:: too many arguments for autoindex");
		}
	}

	void	Host::fetchMethods(std::stringstream& lineStream)
	{
		int			flag = 0;
		std::string	value;

		while (lineStream.good())
		{
			lineStream >> value;
			if (lineStream.fail() || value == "#")
				break ;
			flag = 1;
			if (value == "GET" || value == "POST" || value == "DELETE")
				_methods.insert(value);
			else
				throw std::runtime_error("Host:: invalid method");
		}
		if (flag == 0)
			throw std::runtime_error("Host:: methods is not valid");
	}

	void	Host::fetchBodySize(std::stringstream& lineStream)
	{
		std::string	value;

		lineStream >> value;
		if (value != "#" && ft::isnumber(value) && !lineStream.fail())
			_maxBodySize = ft::stoz(value);
		else
			throw std::runtime_error("Host:: body_size is not valid");
		if (lineStream.good())
		{
			lineStream >> value;
			if (!lineStream.fail() && value != "#")
				throw std::runtime_error("Host:: too many arguments for body_size");
		}
	}

	void	Host::fetchIndexes(std::stringstream& lineStream)
	{
		int			flag = 0;
		std::string	value;

		while (lineStream.good())
		{
			lineStream >> value;
			if (lineStream.fail() || value == "#")
				break ;
			flag = 1;
			_indexes.insert(value);
		}
		if (flag == 0)
			throw std::runtime_error("Host:: invalid index");
	}

	void	Host::fetchErrorPages(std::stringstream& lineStream)
	{
		int			code;
		std::string	value;
		std::string	code_str;

		lineStream >> code_str;
		if (code_str == "#")
			throw std::runtime_error("Host:: invalid error_page");
		if (lineStream.good() && isnumber(code_str))
		{
			code = ft::stoi(code_str);
			try
			{
				HttpStatus::resolve(code);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Host:: invalid error_page code");
			}
			lineStream >> value;
			if (value == "#")
				throw std::runtime_error("Host:: invalid error_page");
			if (!is_path_valid(value))
				throw std::runtime_error("Host:: error_page path is not valid");
			_errorPages[code] = value;
		}
		else
			throw std::runtime_error("Host:: invalid error_page code/page");
		if (lineStream.good())
		{
			lineStream >> value;
			if (!lineStream.fail() && value != "#")
				throw std::runtime_error("Host:: too many arguments for error_page");
		}
	}

	void	Host::fetchLocation(std::ifstream& configFile, std::stringstream& lineStream)
	{
		Location	*loc;
		std::string path;
		std::string	value;
		std::map<std::string, Location *>::iterator	lit;
		
		//opening and clonsing brackets are not handled // done
		lineStream >> path;
		if (lineStream.fail() || path == "#")
			throw std::runtime_error("Host:: invalid location");
		if (lineStream.good())
		{
			lineStream >> value;
			if (!lineStream.fail() && value != "#")
				throw std::runtime_error("Host:: too many arguments for location");
		}
		loc = new Location(configFile, _root, _autoIndex, _indexes, _methods);
		lit = _locations.find(path);
		if (lit != _locations.end())
			delete lit->second;
		_locations[path] = loc;
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

		ostr << std::setw(fieldSize) << "root : " << Host._root << std::endl;
		ostr << std::setw(fieldSize) << "autoIndex : " << Host._autoIndex << std::endl;

		ostr << getDisplayHeader("names", HOST_SHSIZE) << std::endl;
		for (std::set<std::string>::const_iterator it = Host._names.begin(); it != Host._names.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplayFooter(HOST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("methods", HOST_SHSIZE) << std::endl;
		for (std::set<std::string>::const_iterator it = Host._methods.begin(); it != Host._methods.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplayFooter(HOST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("indexes", HOST_SHSIZE) << std::endl;
		for (std::set<std::string>::const_iterator it = Host._indexes.begin(); it != Host._indexes.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplayFooter(HOST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("errorPages", HOST_SHSIZE) << std::endl;
		for (std::map<int, std::string>::const_iterator it = Host._errorPages.begin(); it != Host._errorPages.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplayFooter(HOST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("locations", HOST_SHSIZE) << std::endl;
		for (std::map<std::string, Location *>::const_iterator it = Host._locations.begin(); it != Host._locations.end(); ++it)
			ostr << *it->second << std::endl;
		ostr << getDisplayFooter(HOST_SHSIZE) << std::endl;

		ostr << getDisplayFooter(HOST_HSIZE) << std::endl;
		return ostr;
	}
}