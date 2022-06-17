#include "Location.hpp"

namespace ft
{
	Location::Location()
	{
	}

	Location::~Location()
	{
	}

	Location::Location(std::ifstream& configFile, std::string& root, bool autoIndex, std::set<std::string>& indexes, std::set<std::string>& methods) : _autoIndex(false)
	{
		int				inside_location = 0;
		std::string		line;

		std::getline(configFile, line);
		std::stringstream	streamLine(line);
		std::string			token;
		streamLine >> token;
		if (token != "{")
			throw std::runtime_error("Location:: invalid configuration");
		//{ something is an error : handle it
		while (configFile.good())
		{
			std::getline(configFile, line);
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
			_fetchValue(streamLine, token);
		}
		if (inside_location == 1)
			throw std::runtime_error("Location:: invalid configuration");

		// take server data
		if (_root.empty())
		{
			if (root.empty())
				throw std::runtime_error("Location:: root not found");
			_root = root;
		}
		if (_autoIndex == false)
			_autoIndex = autoIndex;
		if (_indexes.empty())
		{
			if (indexes.empty() && _autoIndex == false) 
				throw std::runtime_error("Location:: erroneous indexing");
			else if (!indexes.empty())
				_indexes = indexes;
		}
		if (_methods.empty())
		{
			if (methods.empty())
				throw std::runtime_error("Location:: methods not found");
			else
				_methods = methods;
		}
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

	void	Location::_fetchValue(std::stringstream& streamLine, std::string& key)
	{
		if (key == "root" && streamLine.good())
			_fetchRoot(streamLine);
		else if (key == "methods" && streamLine.good())
			_fetchMethods(streamLine);
		else if (key == "index" && streamLine.good())
			_fetchIndexes(streamLine);
		else if (key == "upload" && streamLine.good())
			_fetchUploadPath(streamLine);
		else if (key == "autoindex" && streamLine.good())
			_fetchAutoIndex(streamLine);
		else if (key == "return" && streamLine.good())
			_fetchRedirections(streamLine);
	}

	void	Location::_fetchRoot(std::stringstream& streamLine)
	{
		std::string	token;

		streamLine >> token;
		if (token != "#")
			_root = token;
		else
			throw std::runtime_error("Location:: root is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location:: too many arguments for root");
		}
	}

	void	Location::_fetchMethods(std::stringstream& streamLine)
	{
		size_t		size;
		std::string	token;

		size = _methods.size();
		while (streamLine.good())
		{
			streamLine >> token;
			if (token == "#")
				break ;
			if (token == "GET" || token == "POST" || token == "DELETE")
				_methods.insert(token);
			else
				throw std::runtime_error("Location:: method is not valid");
		}
		if (_methods.size() == size)
			throw std::runtime_error("Location:: methods is not valid");
	}

	void	Location::_fetchIndexes(std::stringstream& streamLine)
	{
		size_t		size;
		std::string	token;

		size = _indexes.size();
		while (streamLine.good())
		{
			streamLine >> token;
			if (token == "#")
				break ;
			_indexes.insert(token);
		}
		if (_indexes.size() == size)
			throw std::runtime_error("Location:: indexes is not valid");
	}

	void	Location::_fetchUploadPath(std::stringstream& streamLine)
	{
		std::string	token;

		streamLine >> token;
		if (token != "#")
			_uploadPath = token;
		else
			throw std::runtime_error("Location:: uploadPath is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location:: too many arguments for uploadPath");
		}
	}

	void	Location::_fetchAutoIndex(std::stringstream& streamLine)
	{
		std::string	token;

		streamLine >> token;
		if (token == "on")
			_autoIndex = true;
		else if (token != "off")
			throw std::runtime_error("Location:: autoindex is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location:: too many arguments for autoindex");
		}
	}

	void	Location::_fetchRedirections(std::stringstream& streamLine)
	{
		std::string	token;
		std::string	code_str;

		streamLine >> code_str;
		if (streamLine.good() && isnumber(code_str))
		{
			int code = ::atoi(code_str.c_str());
			try
			{
				HttpStatus::resolve(code);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Location:: return: invalid code");
			}
			streamLine >> token;
			// _redirections = std::make_pair(code, token);!!!!!!!!!!
		}
		else
			throw std::runtime_error("Location:: invalid code/path");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Location:: too many arguments for return");
		}
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
		ostr << std::setw(fieldSize) << "uploadPath : " << location._uploadPath << std::endl;
		ostr << std::setw(fieldSize) << location._redirection.first << " : " << location._redirection.second << std::endl;

		ostr << getDisplayHeader("methods", LOCATION_SHSIZE) << std::endl;
		for (std::set<std::string>::const_iterator it = location._methods.begin(); it != location._methods.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplayFooter(LOCATION_SHSIZE) << std::endl;

		ostr << getDisplayHeader("indexes", LOCATION_SHSIZE) << std::endl;
		for (std::set<std::string>::const_iterator it = location._indexes.begin(); it != location._indexes.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplayFooter(LOCATION_SHSIZE) << std::endl;

		ostr << getDisplayFooter(LOCATION_HSIZE) << std::endl;
		return ostr;
	}
}