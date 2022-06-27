#include "Response.hpp"
#include <cstddef>

namespace ft
{
	Response::Response() : _sent(0), _contentLength(0)
	{
	}

	Response::~Response()
	{
	}

	Response::Response(const Response& src)
	{
		(void)src;
	}

	Response	&Response::operator=(const Response& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	cgi(const Request& request)
	{
		size_t	size;
		char	**envp;


		envp = new char * [size];
		for (size_t i = 0; i < size; i++)
		{
			envp[i] = strdup("env");
		}
		_setEnv("REQUEST_METHOD", request._method);
		_setEnv("SERVER_PROTOCOL", request._version);
		_setEnv("SERVER_PORT", )

	}

	bool	Response::send(int fd)
	{
		int		ret;
		int		len;
		int		left;
		int		size;
		char	buffer[BUFFER_SIZE];
		static std::fstream	logs("response_log", std::ios_base::out | std::ios_base::trunc);

		//header message still has bufferSize or more bytes to send
		if (_sent + BUFFER_SIZE <= _msg.length())
			ret = ::send(fd, _msg.c_str() + _sent, BUFFER_SIZE, 0);
		else
		{
			left = 0;
			if (_sent < _msg.length())
			{
				left = _msg.length() - _sent;
				memmove(buffer, _msg.c_str(), _msg.length() - _sent);
			}
			_body.read(buffer + left, BUFFER_SIZE - left);
			len = _body.gcount();
			size = left + len;
			ret = ::send(fd, buffer, size, 0);
			if (ret > 0 && ret < size)
			{
				if (ret > left)
					len -= size - ret;
				_body.seekg(_body.tellg() - std::streampos(len));
			}
		}
		if (ret < 0)
			throw std::runtime_error("Response:: send failed");
		else if (!ret)
			return true;//should close connection
		else
			_sent += ret;
		return _sent == _contentLength + _msg.length();
	}

	void	Response::getFileFromStatus(const Host *host, int code)
	{
		std::map<int, std::string>::const_iterator	it;

		it = host->_errorPages.find(code);
		if (it == host->_errorPages.end())
			_bodyFileName = "error_pages/" + std::to_string(code) + ".html";
		else
			_bodyFileName = it->second;
	}

	void	Response::build(const std::vector<Host *>& hosts, const Request& request)
	{
		const Host											*host;
		std::map<std::string, std::string>::const_iterator	hostName;

		std::cout << "build response" << std::endl;
		// if (request._bodySize > request._contentLength)
		// {
		// 	_buildBadRequestResponse();
		// 	return ;
		// }
		hostName = request._headers.find("Host");
		// if (hostName == request._headers.end())
		// {
		// 	_buildBadRequestResponse();
		// 	return ;
		// }
		host = _fetchTargetedHost(hosts, hostName->second);
		// _prepare(host, request);
		// if (_status.code != 200 && _bodyFileName.empty())
		// 	getFileFromStatus(host, _status.code);
		//temporary
		_bodyFileName = std::string(NGINY_INDEX_PATH) + "/index.html";
		_status = 200;
		//end temporary
		//check if body exists
		_keepAlive = request._keepAlive;
		_contentLength = getFileSize(_bodyFileName);
		_version = request._version;
		_constructStatusLine();
		_constructHeaders(request);
		_constructBody(request);
	}

	void	Response::_buildBadRequestResponse()
	{
	}

	void	Response::_constructStatusLine()
	{
		std::stringstream	line;
		
		line << _version << " " << _status.code << " " << _status.msg;
		_msg = line.str();
		_msg.append("\r\n");
	}

	void	Response::_constructHeaders(const Request& request)
	{
		std::stringstream	line;

		(void)request;
		line << "Content-Length: " << _contentLength;
		_msg.append(line.str());
		_msg.append("\r\n\r\n");
	}

	void	Response::_constructBody(const Request& request)
	{
		_body.open(_bodyFileName, std::ios_base::in);
		(void)request;
	}

	const Host	*Response::_fetchTargetedHost(const std::vector<Host *>& hosts, const std::string& name)
	{
		for (size_t i = 0; i < hosts.size(); i++)
		{
			if (hosts[i]->hasName(name))
				return hosts[i];
		}
		return hosts[0];
	}

	bool	sort_pair_string(const std::pair<std::string, Location *>& lhs, const std::pair<std::string, Location *>& rhs)
	{
		return lhs.first.length() > rhs.first.length(); // biggest first
	}

	std::pair<std::string, Location *>	Response::get_matched_location_for_request_uri(const std::string path, const std::map<std::string, Location *> locations)
	{
		std::vector<std::pair<std::string, Location *> >	tmp;
		size_t												res;

		for (std::map<std::string, Location *>::const_iterator it = locations.begin(); it != locations.end(); ++it)
			tmp.push_back(std::make_pair(it->first, it->second));
		sort(tmp.begin(), tmp.end(), sort_pair_string);
		for (size_t i = 0; i < tmp.size(); i++)
		{
			res = path.find(tmp[i].first);
			if (res != std::string::npos && res == 0) {
				return std::make_pair(tmp[i].first, tmp[i].second);
			}
		}
		std::string a("");
		Location	*k = NULL;			
		return std::make_pair(a, k);
	}

	bool	Response::is_method_allowded_in_location(const std::string &method, const Location *location)
	{
		for (std::set<std::string>::const_iterator it = location->_methods.begin(); it != location->_methods.end(); ++it)
		{
			if (*it == method)
				return true;
		}
		return false;
	}


	void	Response::_prepare(const Host* host, const Request& request)
	{
		std::pair<std::string, Location *>	location;

		_headers = request._headers;
		location = get_matched_location_for_request_uri(request._path, host->_locations);
		if (location.second == NULL)
		{
			_status = 404;
			return ;
		}
		if (location.second->_redirection.first != 0)
		{
			_status = location.second->_redirection.first;
			_headers["Location"] = location.second->_redirection.second;
			return ;
		}
		if (!is_method_allowded_in_location(request._method, location.second))
		{
			_status = 405;
			return ;
		}
		if (request._method == "GET")
			_handleGetMethod(request, location);
		else if (request._method == "POST")
			_handlePostMethod(request, location);
		else if (request._method == "DELETE")
			_handleDeleteMethod(host, request, location);
	}

	std::string	Response::prepare_path(const std::string& location_root, const std::string &uri)
	{
		std::string	path = location_root;

		if (path[path.length() - 1] != '/' && uri[0] != '/')
		{
			path.append("/");
			path.append(uri);
		}
		else if (path[path.length() - 1] == '/' && uri[0] == '/')
			path.append(uri, 1);
		else
			path.append(uri);
		return path;
	}

	std::string		Response::IsDirHasIndexFiles(const std::pair<std::string, Location *>& location, std::string& path)
	{
		struct stat s;
		std::string	tmp;
		std::string	nfound("");

		if (location.second->_indexes.size())
		{
			for (std::set<std::string>::const_iterator it = location.second->_indexes.begin(); it != location.second->_indexes.end(); ++it)
			{
				tmp = path + *it;
				if (stat(tmp.c_str(), &s) == 0)
					return *it;
			}
		}
		return nfound;
	}

	void	Response::_handleFileInGet(const std::pair<std::string, Location *>& location, std::string& path)
	{
		(void)location;
		// if (if_location_has_cgi())
		// {}
		// else
		{
			_status = 200;
			_bodyFileName = path;
			return ;
		}
	}

	void	Response::_handleDirInGet(const std::pair<std::string, Location *>& location, std::string& path)
	{
		std::string		index_file;

		if (path[path.length() - 1] != '/')
		{
			_status = 301;
			_headers["Location"] = path.append("/");
			return ;
		}
		index_file = IsDirHasIndexFiles(location, path);
		if (index_file.length())
			_handleFileInGet(location, index_file);
		else if (location.second->_autoIndex == true)
		{
			_status = 200;
			// _bodyFileName = path;
		}
		else
			_status = 403;
	}

	void	Response::_handleGetMethod(const Request& request, const std::pair<std::string, Location *>& location)
	{
		struct stat s;
		std::string	path;

		path = prepare_path(location.second->_root, request._path.substr(location.first.length()));
		if(stat(path.c_str(), &s) == 0)
		{
			if(s.st_mode & S_IFDIR)		//it's a directory
				_handleDirInGet(location, path);
			else
				_handleFileInGet(location, path);
		}
		else
			_status = 404;
	}

	void	Response::_handleDirIn_POST(const std::pair<std::string, Location *>& location, std::string& path)
	{
		std::string		index_file;

		if (path[path.length() - 1] != '/')
		{
			_status = 301;
			_headers["Location"] = path.append("/");
			return ;
		}
		index_file = IsDirHasIndexFiles(location, path);
		if (index_file.length())
			_handleFileIn_POST(location, index_file);
		else
			_status = 403;
	}

	void	Response::_handleFileIn_POST(const std::pair<std::string, Location *>& location, std::string& path)
	{
		(void)location;//mathayedhach bash ytcompila
		(void)path;//mathayedhach bash ytcompila
		// if (if_location_has_cgi())
		// {}
		// else
		{
			_status = 403;
			return ;
		}
	}

	void	Response::_uploadfile(const Request& request, const std::string& path)
	{
		std::string		file_uploaded_name;
		std::ofstream	file;
		std::ifstream	read_from;
		char			*buffer;
		int				buffer_size = 1048576;
		size_t			readedsize = 0;
		size_t			file_size = getFileSize(request._bodyFileName);

		file_uploaded_name = path + "/" + ft::getRandomFileName();
		file.open(file_uploaded_name.c_str());
		if (!file.is_open())
		{
			_status = 500;
			return ;
		}
		read_from.open(request._bodyFileName.c_str());
		if (!read_from.is_open())
		{
			_status = 500;
			file.close();
			return ;
		}
		buffer = (char *)malloc(1048576);
		if (!buffer)
		{
			_status = 500;
			file.close();
			read_from.close();
			return ;
		}
		while (read_from.good())
		{
			read_from.read(buffer, buffer_size);
			file.write(buffer, buffer_size);
			readedsize += buffer_size;
			if (file_size - readedsize < buffer_size)
				buffer_size = file_size - readedsize;
		}
		free(buffer);
		file.close();
		read_from.close();
		_status = 201;
	}

	void	Response::_handlePostMethod(const Request& request, const std::pair<std::string, Location *>& location)
	{
		struct stat s;
		std::string	path;

		if (location.second->_uploadPath.length() != 0)
		{
			// uplaod the Post Request Body
			_uploadfile(request, location.second->_uploadPath);
			return ;
		}
		path = prepare_path(location.second->_root, request._path.substr(location.first.length()));
		if(stat(path.c_str(), &s) == 0)
		{
			if(s.st_mode & S_IFDIR)		//it's a directory
				_handleDirIn_POST(location, path);
			else
				_handleFileIn_POST(location, path);
		}
		else
			_status = 404;
	}
 
	int	DeleteFolderContent(std::string& path)
	{
		DIR				*d;
		struct dirent	de, *dep; 
		int				i;
		int				ret;
		std::string		path2;

		if (!isFileWritable(path))
			return (403);
		d = opendir(path.c_str());
		if (d == NULL) 
			return (500);
		i = 0;
		while (readdir_r(d, &de, &dep) == 0 && dep != NULL)
		{
			if (strcmp(de.d_name, ".") == 0 || strcmp(de.d_name, "..") == 0)
				continue ;
			path2 = path + "/" + de.d_name;
			if (de.d_type == 4) // dir
			{
				ret = DeleteFolderContent(path2);
				if (ret != 204)
					return (ret);
				if (rmdir(path2.c_str()) < 0)
					return (500);
			}
			else
			{
				if (!isFileWritable(path2))
					return (403);
				if (unlink(path2.c_str()) < 0)
					return (500);
			}
		}
		if (closedir(d) == -1)
			return (500);
		return (204);
	}

	void	Response::_handleDirIn_DELETE(const std::pair<std::string, Location *>& location, std::string& path)
	{
		std::string		index_file;
		int				ret;

		(void)location;//mathayedhach bash ytcompila
		if (path[path.length() - 1] != '/')
		{
			_status = 409;
			return ;
		}
							//			CGI
		// if (if_location_has_cgi())
		// {
		// 	index_file = IsDirHasIndexFiles(location, path);
		// 	if (index_file.length())
		// 	{
		// 		// run cgi  on requested file with DELTE REQUEST_METHOD
		// 	}
		// 	else
		// 	{
		// 		_status = 403;
		// 	}
		// }
		// else
		{
			ret = DeleteFolderContent(path);
			if (ret == 204)
			{
				if (rmdir(path.c_str()) < 0)
					_status = 500;
				else
				 	_status = 204;
			}
			else
				_status = ret;
		}
	}

	void	Response::_handleFileIn_DELETE(const std::pair<std::string, Location *>& location, std::string& path)
	{
		(void)location;//mathayedhach bash ytcompila

		// if (if_location_has_cgi())
		// {
		// 	// run cgi  on requested file with DELTE REQUEST_METHOD
		// }
		// else
		{
			if (!isFileWritable(path))
				_status = 403;
			else if (unlink(path.c_str()) < 0)
				_status = 500;
			else
				_status = 204;
		}
	}

	void	Response::_handleDeleteMethod(const Host* host, const Request& request, const std::pair<std::string, Location *>& location)
	{
		(void)host;

		struct stat s;
		std::string	path;

		path = prepare_path(location.second->_root, request._path.substr(location.first.length()));
		if(stat(path.c_str(), &s) == 0)
		{
			if(s.st_mode & S_IFDIR)		//it's a directory
				_handleDirIn_DELETE(location, path);
			else
				_handleFileIn_DELETE(location, path);
		}
		else
			_status = 404;
	}

	void	Response::reset()
	{
		_sent = 0;
		_contentLength = 0;
		_msg.clear();
		_version.clear();
		_status = 200;
		_headers.clear();
		_bodyFileName.clear();
		_body.close();
	}

	void	Response::_deepCopy(const Response& src)
	{
		(void)src;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Response& response)
	{
		const int		fieldSize = 30;
		std::string		line;
		std::ifstream	bodyFile;

		ostr << std::left;
		ostr << getDisplayHeader("Response", RESPONSE_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "version : " << response._version << std::endl;
		ostr << std::setw(fieldSize) << "status : " << response._status.code << " : " << response._status.msg << std::endl;

		ostr << getDisplayHeader("headers", RESPONSE_SHSIZE) << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = response._headers.begin(); it != response._headers.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplayFooter(RESPONSE_SHSIZE) << std::endl;

		ostr << getDisplayHeader("body", RESPONSE_SHSIZE) << std::endl;
		ostr << "name : " << response._bodyFileName << std::endl;
		ostr << "=============================================================================" << std::endl;
		// bodyFile.open((response._bodyFileName.c_str()));
		// while (bodyFile.good())
		// {
		// 	std::getline(bodyFile, line);
		// 	ostr << line << std::endl;
		// }
		// bodyFile.close();
		ostr << "=============================================================================" << std::endl;
		ostr << getDisplayFooter(RESPONSE_SHSIZE) << std::endl;
		
		ostr << getDisplayFooter(RESPONSE_HSIZE) << std::endl;
		return ostr;
	}
}