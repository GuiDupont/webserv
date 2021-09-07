

#include "location.hpp"
#include <iostream>

size_t g_line;


location::location(std::ifstream & config_file, std::string & line) : _max_body_size(0), _disable_methods(0) {

	int i = go_to_next_word(line, 0);
	std::string path = get_word(line, i);
	i = go_to_next_word(line, i);
	std::string opening_bracket = get_word(line, i);
	if (opening_bracket != "{")
		throw (bad_location_declaration());
	_path = path;
	std::string	first_word;
	while (1) {
		std::getline(config_file, line, '\n');
		g_line++;
		i = 0;
		while (isspace(line[i])) 
			i++;
		first_word = line.substr(i, line.find_first_of(" \t\n\v\f\r", i) - i);
		if (first_word.empty())
			continue;
		if (first_word == "disable_methods") // moi
			;
		else if (first_word == "return")
			;
		else if (first_word == "cgi_dir")
			;
		else if (first_word == "root") // moi
			;
		else if (first_word == "upload_pass")
			;
		else if (first_word == "auto_index") // moi
			;
		else if (first_word == "error_page") // moi
			;
		else if (first_word == "client_max_body_size")
			;
		else if (first_word == "}")
			return ;
		else
		{
			std::cout << "bad directive : " << first_word << std::endl;
			throw (bad_directive());
		}
	}
}