#include "Response.hpp"

#include "../server/Server.hpp"

namespace ft
{
	Response::Response() :	_sent(0), _msg(), _cgi(), _keepAlive(true), _contentLength(0), _version("HTTP/1.1"),
							_status(), _headers(), _bodyFileName(), _body()
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

	bool	Response::timeOut()
	{
		Cgi::Status	ret;

		ret = _cgi.timeOut();
		if (ret == Cgi::cTimeout)
			return true;
		else if (ret == Cgi::cError)
			_constructErrorResponse(, 500);
		return false;
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

		if (host)
		{
			it = host->_errorPages.find(code);
			if (it != host->_errorPages.end())
			{
				_bodyFileName = it->second;
				return ;
			}
		}
		_bodyFileName = NGINY_ERROR_PAGES_PATH + "/" + std::to_string(code) + ".html";
	}

	void	Response::build(const std::vector<Host *>& hosts, const Request& request)
	{
		const Host											*host;
		std::map<std::string, std::string>::const_iterator	hostName;

		host = _fetchTargetedHost(hosts, hostName->second);
		if (request.status() != Request::good)
			_constructErrorResponse(host, 400);
		else
		{
			_cgi.setHost(host);

			_constructHead(request);

			// _prepare(host, request);

			//temporary
			_bodyFileName = std::string(NGINY_INDEX_PATH) + "/index.html";
			_status = 200;
			//end temporary
		}
		_contentLength = getFileSize(_bodyFileName);
			
		_constructStatusLine();
		_constructHeaders();
		_constructBody();
	}

	void	Response::_constructErrorResponse(const Host *host, const HttpStatus& status)
	{
		_status = status;
		getFileFromStatus(host, _status.code);
	}

	void	Response::_constructHead(const Request& request)
	{
		_headers = request._headers;

		//remove request headers related to content framing
		_headers.erase("Content-Type");
		_headers.erase("Content-Length");
		_headers.erase("Transfer-Encoding");
		_headers.erase("Host");
		_headers.erase("Trailer");

		_keepAlive = request._keepAlive;
		_version = request._version;
	}

	void	Response::_constructStatusLine()
	{
		std::stringstream	line;
		
		line << _version << " " << _status.code << " " << _status.msg;
		_msg = line.str();
		_msg.append("\r\n");
	}

	void	Response::_constructHeaders()
	{
		std::stringstream	line;
		std::map<std::string, std::string>::iterator	it;

		line << "Content-Length: " << _contentLength;
		_msg.append(line.str() + "\r\n");
		for (it = _headers.begin(); it != _headers.end(); ++it)
			_msg.append(it->first + ": " + it->second + "\r\n");
		_msg.append("\r\n\r\n");
	}

	void	Response::_constructBody()
	{
		_body.open(_bodyFileName, std::ios_base::in);
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
		size_t												res;
		std::vector<std::pair<std::string, Location *> >	tmp;

		for (std::map<std::string, Location *>::const_iterator it = locations.begin(); it != locations.end(); ++it)
			tmp.push_back(std::make_pair(it->first, it->second));
		sort(tmp.begin(), tmp.end(), sort_pair_string);
		for (size_t i = 0; i < tmp.size(); i++)
		{
			res = path.find(tmp[i].first);
			if (res != std::string::npos && res == 0)
				return std::make_pair(tmp[i].first, tmp[i].second);
		}
		return std::make_pair<std::string, Location*>(std::string(), NULL);
	}

	bool	Response::is_method_allowded_in_location(const std::string &method, const Location *location)
	{
		std::set<std::string>::const_iterator it;

		it  = location->_methods.find(method);
		return it != location->_methods.end();
	}


	void	Response::_prepare(const Host* host, const Request& request)
	{
		std::pair<std::string, Location *>	location;

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
		std::string	path(location_root);
		
		if (path[path.length() - 1] != '/' && uri[0] != '/')
			path.append("/" + uri);
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

		if (location.second->_indexes.size())
		{
			for (std::set<std::string>::const_iterator it = location.second->_indexes.begin(); it != location.second->_indexes.end(); ++it)
			{
				tmp = path + *it;
				if (stat(tmp.c_str(), &s) == 0)
					return *it;
			}
		}
		return std::string();
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

	void	Response::_prepare_indixng(std::string& path)
	{
		std::string		file_name;
		std::string		path2;
		std::ofstream	file;
		DIR				*d;
		struct dirent	de, *dep; 

		file_name = "/tmp/" + ft::getRandomFileName() + ".html";
		file.open(file_name.c_str());
		if (!file.is_open())
		{
			_status = 500;
			return ;
		}
		d = opendir(path.c_str());
		if (d == NULL)
		{
			_status = 500;
			return ;
		}

		char	*tmp = ft_strdup("<!DOCTYPE html>\n<html>\n    <head>\n        <title>");
		file.write(tmp, strlen(tmp));
		delete [] tmp;
		file.write(path.c_str(), path.length());
		tmp = ft_strdup("</title>\n        <style type=\"text/css\">\n            h1 {border-bottom: 1px solid #c0c0c0;margin-bottom: 10px;padding-bottom: 10px;white-space: nowrap;}\n\
				tr {display: table-row;vertical-align: inherit;border-color: inherit;}\n\
				th {display: table-cell;vertical-align: inherit;font-weight: bold;text-align: -internal-center;}\n\
				td {display: table-cell;vertical-align: inherit;}\n\
				table {border-collapse: collapse;}\n\
				table {border-collapse: separate;text-indent: initial;border-spacing: 2px;}\n\
				thead {display: table-header-group;vertical-align: middle;border-color: inherit;}\n\
				a.dir {\n\
				background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAABt0lEQVR42oxStZoWQRCs2cXdHTLcHZ6EjAwnQWIkJyQlRt4Cd3d3d1n5d7q7ju1zv/q+mh6taQsk8fn29kPDRo87SDMQcNAUJgIQkBjdAoRKdXjm2mOH0AqS+PlkP8sfp0h93iu/PDji9s2FzSSJVg5ykZqWgfGRr9rAAAQiDFoB1OfyESZEB7iAI0lHwLREQBcQQKqo8p+gNUCguwCNAAUQAcFOb0NNGjT+BbUC2YsHZpWLhC6/m0chqIoM1LKbQIIBwlTQE1xAo9QDGDPYf6rkTpPc92gCUYVJAZjhyZltJ95f3zuvLYRGWWCUNkDL2333McBh4kaLlxg+aTmyL7c2xTjkN4Bt7oE3DBP/3SRz65R/bkmBRPGzcRNHYuzMjaj+fdnaFoJUEdTSXfaHbe7XNnMPyqryPcmfY+zURaAB7SHk9cXSH4fQ5rojgCAVIuqCNWgRhLYLhJB4k3iZfIPtnQiCpjAzeBIRXMA6emAqoEbQSoDdGxFUrxS1AYcpaNbBgyQBGJEOnYOeENKR/iAd1npusI4C75/c3539+nbUjOgZV5CkAU27df40lH+agUdIuA/EAgDmZnwZlhDc0wAAAABJRU5ErkJggg==) left top no-repeat;}\n\
				a.icon {padding-inline-start: 1.5em;text-decoration: none;user-select: auto;}\n\
				a:-webkit-any-link {color: -webkit-link;cursor: pointer;text-decoration: underline;}\n\
				a.file {\n\
				background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAIAAACQkWg2AAAABnRSTlMAAAAAAABupgeRAAABEElEQVR42nRRx3HDMBC846AHZ7sP54BmWAyrsP588qnwlhqw/k4v5ZwWxM1hzmGRgV1cYqrRarXoH2w2m6qqiqKIR6cPtzc3xMSML2Te7XZZlnW7Pe/91/dX47WRBHuA9oyGmRknzGDjab1ePzw8bLfb6WRalmW4ip9FDVpYSWZgOp12Oh3nXJ7nxoJSGEciteP9y+fH52q1euv38WosqA6T2gGOT44vry7BEQtJkMAMMpa6JagAMcUfWYa4hkkzAc7fFlSjwqCoOUYAF5RjHZPVCFBOtSBGfgUDji3c3jpibeEMQhIMh8NwshqyRsBJgvF4jMs/YlVR5KhgNpuBLzk0OcUiR3CMhcPaOzsZiAAA/AjmaB3WZIkAAAAASUVORK5CYII=) left top no-repeat;\n\
				}\n\
				a.icon {padding-inline-start: 1.5em;text-decoration: none;user-select: auto;}\n\
				a:-webkit-any-link {color: -webkit-link;cursor: pointer;text-decoration: underline;}\n\
			</style>\n\
		</head>\n\
		<body>\n\
			<h1>");
		file.write(tmp, strlen(tmp));
		delete [] tmp;
		file.write(path.c_str(), path.length());

		tmp = ft_strdup("</h1>\n        <table>\n            <thead>\n                <tr class=\"header\" id=\"theader\">\n\
						<th id=\"nameColumnHeader\" tabindex=\"0\" role=\"button\">Name</th>  <br /><br />\n\
					</tr>\n            </thead>\n            <tbody id=\"tbody\">\n");
		file.write(tmp, strlen(tmp));
		delete [] tmp;

		while (readdir_r(d, &de, &dep) == 0 && dep != NULL)
		{
			if (strcmp(de.d_name, ".") == 0)
				continue ;
			path2 = path + de.d_name;
			if (de.d_type == 4) // dir
			{
				tmp = ft_strdup("                <tr><td>\n                        <a class=\"icon dir\" href=\"");
				file.write(tmp, strlen(tmp));
				delete [] tmp;
				file.write(path2.c_str(), path2.length());
				file.write("/\">", 3);
				if (strcmp(de.d_name, "..") == 0)
					file.write("[parent directory]</a>\n                    </td></tr>\n", strlen("[parent directory]</a>\n                    </td></tr>\n"));
				else
				{
					file.write(de.d_name, strlen(de.d_name));
					tmp = ft_strdup("/</a>\n                    </td></tr>\n");
					file.write(tmp, strlen(tmp));
					delete [] tmp;
				}
			}
			else
			{
				tmp = ft_strdup("                <tr><td>\n                        <a class=\"icon file\" draggable=\"true\" href=\"");
				file.write(tmp, strlen(tmp));
				delete [] tmp;
				file.write(path2.c_str(), path2.length());
				file.write("\">", 3);
				file.write(de.d_name, strlen(de.d_name));
				tmp = ft_strdup("</a>\n                    </td></tr>\n");
				file.write(tmp, strlen(tmp));
				delete [] tmp;
			}
		}

		tmp = ft_strdup("            </tbody>\n        </table>\n    </body>\n</html>");
		file.write(tmp, strlen(tmp));
		delete [] tmp;

		if (closedir(d) == -1)
		{
			_status = 500;
			return ;
		}
		_bodyFileName = file_name;
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
			_prepare_indixng(path);
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
		size_t			buffer_size = UPLOAD_BUFFER_SIZE;
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
		buffer = new (std::nothrow) char[UPLOAD_BUFFER_SIZE];
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
			// read_from.fail()
			file.write(buffer, buffer_size);
			readedsize += buffer_size;
			if (file_size - readedsize < buffer_size)
				buffer_size = file_size - readedsize;
		}
		delete [] buffer;
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
 
	int	Response::DeleteFolderContent(std::string& path)
	{
		DIR				*d;
		struct dirent	de, *dep; 
		int				ret;
		std::string		path2;

		if (!isFileWritable(path))
			return (403);
		d = opendir(path.c_str());
		if (d == NULL) 
			return (500);
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

	void	Response::_initiateCgi(Request& request, const std::string& scriptName, const std::string& pathInfo, const std::string& pathTranslated)
	{
		_cgi.setPathInfo(pathInfo);
		_cgi.setScriptName(scriptName);
		_cgi.setPathTranslated(pathTranslated);

		_cgi.execute(*this, request);
	}

	void	Response::reset()
	{
		_sent = 0;
		_msg.clear();

		_cgi.reset();
		_isReady = false;

		_keepAlive = true;
		_contentLength = 0;

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