#ifndef STDLIB_HPP
# define STDLIB_HPP

#include <string>
#include <vector>
#include <sstream>

// #define NGINY_CONF_PATH	""
// #define NGINY_VAR_PATH		""
// #define NGINY_INDEX_PATH	""

#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

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
	bool	isnumber(const std::string &str);
	size_t	stoz(const std::string& str);
	size_t	hstoz(const std::string& str);
	std::string	ft_itoa(int n);
	

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

	std::string	getRandomFileName();
}

#endif