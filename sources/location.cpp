

#include "location.hpp"
#include <iostream>

size_t g_line;


location::location(std::ifstream & config_file, std::string & line) : _client_max_body_size(0), _disabled_methods(0), _auto_index(false) {
	std::cout << "parsing a location context\n";
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
		if (first_word == "disabled_methods")
			_disabled_methods = parse_disabled_methods(line);
		else if (first_word == "return")
			;
		else if (first_word == "cgi_dir")
			;
		else if (first_word == "root")
			_root = parse_root(line);
		else if (first_word == "upload_pass")
			;
		else if (first_word == "autoindex")
			_auto_index = parse_auto_index(line);
		else if (first_word == "error_page")
			std::cout << parse_error_page(line).first;
		else if (first_word == "client_max_body_size")
			_client_max_body_size = get_max_body_size(line);
		else if (first_word == "}")
			return ;
		else
		{
			std::cout << "bad directive : " << first_word << std::endl;
			throw (bad_directive());
		}
	}
}

size_t	location::parse_disabled_methods(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string method;
	size_t disabled_methods = 0;
	while (1) {
		method = get_word(line, i);
		if (method == "GET")
			disabled_methods |= GET;
		else if (method == "POST")
			disabled_methods |= POST;
		else if (method == "DELETE")
			disabled_methods |= DELETE;
		else
			throw (non_existing_disabled_methods());
		i = go_to_next_word(line, i);
		if (!line[i])
			return (disabled_methods);
	}
	
	return (disabled_methods);
}

std::string	location::parse_root(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());// change with empty 
	std::string path = get_word(line, i);
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_directive()); // change with wrong nb_arg
	std::cout << path << std::endl;
	return (path);
}


bool	location::parse_auto_index(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string on_off = get_word(line, i);
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_directive()); //change with wrong nb argument
	if (on_off == "on")
		return true;
	if (on_off == "off")
		return false;
	else
		throw (bad_auto_index_value()); //change with wrong nb argument

}