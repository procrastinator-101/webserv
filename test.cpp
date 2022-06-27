#include "srcs/response/Response.hpp"
#include "srcs/stdlib/stdlib.hpp"
#include <cstring>
#include <string>
#include <sys/dirent.h>
#include <unistd.h>

char	*ft_strdup(const char *s1)
{
	size_t		len;
	char		*ret;

	len = strlen(s1);
	ret = new char[len + 1];
	for (size_t i = 0; i < len; i++)
		ret[i] = s1[i];
	ret[len] = 0;
	return ret;
}

void	prepare_indixng(std::string& path)
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
		//_status = 500;
		return ;
	}
	d = opendir(path.c_str());
	if (d == NULL)
	{
		//_status = 500;
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
		//_status = 500;
		return ;
	}
}

int main()
{
	std::string path("/Users/hhoummad/");
	prepare_indixng(path);
}