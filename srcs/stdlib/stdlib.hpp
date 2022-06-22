#ifndef STDLIB_HPP
# define STDLIB_HPP

#include <string>
#include <vector>
#include <sstream>

// #define NGINY_CONF_PATH	""
// #define NGINY_VAR_PATH		""
// #define NGINY_INDEX_PATH	""

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
	
	std::vector<std::string>	splitWhiteSpaces(const std::string& str);
	std::vector<std::string>	split(const std::string& str, char delimiter);
	std::vector<std::string>	split(const std::string& str, const std::string& delimiter);
	std::string	strtok(std::string& str, int delimiter = -1);

	std::string	removeTrailingWhiteSpaces(const std::string& str);

	std::string	centerString(int width, const std::string& str, char c);
	
	std::string	getDisplayFooter(size_t n);
	std::string	getDisplayHeader(std::string str, size_t n);

	std::string	getDisplaySubHeader(std::string str);
	std::string	getDisplaySubFooter(std::string str);

	std::string	getRandomFileName();
	std::string	ft_itoa(int n);

	size_t	getFileSize(const std::string& fileName);
}

#endif