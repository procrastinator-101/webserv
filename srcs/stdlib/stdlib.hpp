#ifndef STDLIB_HPP
# define STDLIB_HPP

#include <string>
#include <sys/_types/_size_t.h>
#include <vector>
#include <sstream>

#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

#include <cstring>
#include <_ctype.h>
#include <exception>
#include <stdexcept>
#include <iostream>

#include <sys/time.h>

//level 1
//-------------------------------------------------------
#define SOCKT_HSIZE			20
#define SOCKT_SHSIZE		15
//-------------------------------------------------------


//level 2
//-------------------------------------------------------
#define REQUEST_HSIZE		30
#define REQUEST_SHSIZE		25

#define RESPONSE_HSIZE		30
#define RESPONSE_SHSIZE		25
//-------------------------------------------------------

//level 3
//-------------------------------------------------------
#define LOCATION_HSIZE		40
#define LOCATION_SHSIZE		35

#define CLIENT_HSIZE		40
#define CLIENT_SHSIZE		35
//-------------------------------------------------------

//level 4
//-------------------------------------------------------
#define HOST_HSIZE			50
#define HOST_SHSIZE			45
//-------------------------------------------------------

//level 5
//-------------------------------------------------------
#define SERVER_HSIZE		60
#define SERVER_SHSIZE		55

#define SELECTER_HSIZE		60
#define SELECTER_SHSIZE		55
//-------------------------------------------------------

//level 6
//-------------------------------------------------------
#define NGINY_HSIZE			70
#define NGINY_SHSIZE		65
//-------------------------------------------------------


namespace ft
{
	long	getTimeStamp(timeval start);

	char	decodePercent(std::string& src);


	int	stoi(const std::string& str);
	bool	isnumber(const std::string &str);
	size_t	stoz(const std::string& str);
	size_t	hstoz(const std::string& str);
	std::string	itoa(int n);
	std::string	ztoa(size_t n);

	char	*ft_strdup(const char *s1);
	char	**vec2arr(std::vector<std::string>& vec);
	void	destroy2arr(char **arr, size_t n);

	bool	is_path_valid(const std::string& path);
	
	std::string	trim(const std::string& str, const std::string& delimiters);
	std::string	ltrim(const std::string& str, const std::string& delimiters);
	std::string	rtrim(const std::string& str, const std::string& delimiters);
	
	std::vector<std::string>	split(const std::string& str, const std::string& delimiter);
	std::vector<std::string>	mtsplit(const std::string& str, const std::string& delimiters);

	std::string	strtok(std::string& str, const std::string& delimiters);
	std::string	strdtok(std::string& str, const std::string& delimiter);

	std::string	removeTrailingWhiteSpaces(const std::string& str);


	std::string	centerString(int width, const std::string& str, char c);
	
	std::string	getDisplayFooter(size_t n);
	std::string	getDisplayHeader(std::string str, size_t n);

	std::string	getDisplaySubHeader(std::string str);
	std::string	getDisplaySubFooter(std::string str);


	bool	isFileWritable(std::string& name);
	bool	isUserInGroup(gid_t gid, uid_t uid);
	size_t	getFileSize(const std::string& name);

	std::string	getRandomFileName(const std::string& extension);

	namespace http
	{
		bool	isTchar(int c);
		bool	isCtext(int c);
		bool	isQdtext(int c);
		bool	isObstext(int c);
		bool	isDelimiter(int c);

		std::string	headerToEnv(const std::string& header);
	}
}

#endif