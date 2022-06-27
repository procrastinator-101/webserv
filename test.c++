#include "srcs/response/Response.hpp"
#include <string>

void	prepare_indixng(std::string& path)
{
	std::string		file_name;
	std::ofstream	file;
	DIR				*d;
	struct dirent	de, *dep; 

	file_name = "/tmp/" + ft::getRandomFileName() + ".html";
	file.open(file_name.c_str());
	if (!file.is_open())
	{
		//_status = 500;
		return ;
	}
	d = opendir(path.c_str());
	if (d == NULL)
	{
		//_status = 500;
		return ;
	}
	char	*tmp = strdup("<!DOCTYPE html>\n<html>\n    <head>\n        <title>");
	file.write(tmp, strlen(tmp));
	
	while (readdir_r(d, &de, &dep) == 0 && dep != NULL)
	{
		if (strcmp(de.d_name, ".") == 0)
			continue ;
		if (de.d_type == 4) // dir
		{
			
		}
		else
		{
			
		}
	}
	if (closedir(d) == -1)
	{
		//_status = 500;
		return ;
	}
}

int main()
{
	std::string path("/Users/hhoummad/Desktop/webs");
	prepare_indixng(path);
}