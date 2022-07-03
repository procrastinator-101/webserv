#include "Location.hpp"
#include <cstdio>
#include <sys/_types/_size_t.h>

namespace ft
{
	Location::Location() :	_root(), _autoIndex(false), _methods(), _indexes(), _redirection(), _cgis(), _uploadPath()
	{
	}

	Location::~Location()
	{
	}

	Location::Location(std::ifstream& configFile, std::string& root, bool autoIndex, std::set<std::string>& indexes, std::set<std::string>& methods) :	_root(), _autoIndex(autoIndex), _methods(),
																																						_indexes(), _redirection(), _cgis(),
																																						_uploadPath()
	{
		int				inside_location = 0;
		std::string		line;

		std::getline(configFile, line);
		std::stringstream	streamLine(line);
		std::string			token;

		streamLine >> token;
		if (token != "{")
			throw std::runtime_error("Location:: invalid configuration");
		if (streamLine.good())
		{
			streamLine >> token;
			if (!streamLine.fail() && token != "#")
				throw std::runtime_error("Location:: invalid configuration");
		}

	//init some values
		int		init_value = 0;
		_redirection = std::make_pair(init_value, "");
		_uploadPath = "";

		//{ something is an error : handle it // I think done
		while (configFile.good())
		{
			std::getline(configFile, line);
			inside_location = 1;
			if (line.empty())
				continue ;
			streamLine.clear();
			streamLine.str(line);
			streamLine >> token;
			if (streamLine.fail() || token == "#")
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
		else if (key == "cgi" && streamLine.good())
			_fetchCgi(streamLine);
		else
			throw std::runtime_error("Server:: invalid key {" + key + "}");
	}

	void	Location::_fetchRoot(std::stringstream& streamLine)
	{
		std::string	token;

		streamLine >> token;
		if (!streamLine.fail() && token != "#")
			_root = token;
		else
			throw std::runtime_error("Location:: root is not valid");
		if (!is_path_valid(_root))
			throw std::runtime_error("Location:: root is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (!streamLine.fail() && token != "#")
				throw std::runtime_error("Location:: too many arguments for root");
		}
	}

	void	Location::_fetchMethods(std::stringstream& streamLine)
	{
		int			flag = 0;
		std::string	token;

		while (streamLine.good())
		{
			streamLine >> token;
			if (streamLine.fail() || token == "#")
				break ;
			flag = 1;
			if (token == "GET" || token == "POST" || token == "DELETE")
				_methods.insert(token);
			else
				throw std::runtime_error("Location:: method is not valid");
		}
		if (flag == 0)
			throw std::runtime_error("Location:: methods is not valid");
	}

	void	Location::_fetchIndexes(std::stringstream& streamLine)
	{
		int			flag = 0;
		std::string	token;

		while (streamLine.good())
		{
			streamLine >> token;
			if (streamLine.fail() || token == "#")
				break ;
			flag = 1;
			_indexes.insert(token);
		}
		if (flag == 0)
			throw std::runtime_error("Location:: index is not valid");
	}

	void	Location::_fetchUploadPath(std::stringstream& streamLine)
	{
		std::string	token;
		struct stat s;

		streamLine >> token;
		if (!streamLine.fail() && token != "#")
		{
			if (stat(token.c_str(), &s) == 0)
				_uploadPath = token;
			else
				throw std::runtime_error("Location:: uploadPath is not valid");
		}
		else
			throw std::runtime_error("Location:: uploadPath is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (!streamLine.fail() && token != "#")
				throw std::runtime_error("Location:: too many arguments for uploadPath");
		}
	}

	void	Location::_fetchAutoIndex(std::stringstream& streamLine)
	{
		std::string	token;

		streamLine >> token;
		if (!streamLine.fail() && token == "on")
			_autoIndex = true;
		else if (streamLine.fail() || token != "off")
			throw std::runtime_error("Location:: autoindex is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (!streamLine.fail() && token != "#")
				throw std::runtime_error("Location:: too many arguments for autoindex");
		}
	}

	void	Location::_fetchRedirections(std::stringstream& streamLine)//to check
	{
		std::string	token;
		std::string	code_str;

		streamLine >> code_str;
		if (streamLine.good() && isnumber(code_str))
		{
			// int code = ::atoi(code_str.c_str());
			int	code = ft::stoi(code_str);
			if (code < 300 || code > 399)
				throw std::runtime_error("Location:: return: invalid code");
			try
			{
				HttpStatus::resolve(code);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Location:: return: invalid code");
			}
			streamLine >> token;
			if (!streamLine.fail() && token != "#")
				_redirection = std::make_pair(code, token);
			else
				throw std::runtime_error("Location:: return: invalid path");
		}
		else
			throw std::runtime_error("Location:: invalid code/path");
		if (streamLine.good())
		{
			streamLine >> token;
			if (!streamLine.fail() && token != "#")
				throw std::runtime_error("Location:: too many arguments for return");
		}
	}

	void	Location::_fetchCgi(std::stringstream& streamLine)
	{
		std::string path;
		std::string ext;
		std::string	value;

		streamLine >> ext;
		if (streamLine.fail() || ext == "#")
			throw std::runtime_error("Location:: invalid CGI");
		streamLine >> path;
		if (streamLine.fail() || path == "#")
			throw std::runtime_error("Location:: invalid CGI");
		if (streamLine.good())
		{
			streamLine >> value;
			if (!streamLine.fail() && value != "#")
				throw std::runtime_error("Location:: too many arguments for CGI");
		}
		if (!is_path_valid(path))
			throw std::runtime_error("Location:: cgi path is not valid");
		_cgis[ext] = path;
	}

	void	Location::_deepCopy(const Location& src)
	{
		_root = src._root;
		_autoIndex = src._autoIndex;
		_methods = src._methods;
		_indexes = src._indexes;
		_redirection = src._redirection;
		_cgis = src._cgis;
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

		ostr << getDisplayHeader("cgis", LOCATION_SHSIZE) << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = location._cgis.begin(); it != location._cgis.end(); ++it)
			ostr << it->first << " : " << it->second << std::endl;
		ostr << getDisplayFooter(LOCATION_SHSIZE) << std::endl;

		ostr << getDisplayFooter(LOCATION_HSIZE) << std::endl;
		return ostr;
	}
}