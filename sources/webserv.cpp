/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/07 10:18:50 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include <string>
#include <fstream>
#include "../includes/exceptions.hpp"
#include <iostream>


webserv::webserv(const std::string & path_config) : auto_index(false), client_max_body_size(1) {
	std::ifstream	config_file;
	std::string		all_file;
	
	config_file.open(path_config.c_str(), std::ios_base::in);
	if (!config_file.is_open())
		throw (config_file_not_open());
	std::getline(config_file, all_file, '\0');
	if (!this->check_brackets(all_file))
		throw (bad_brackets_conf());
	this->set_config(config_file);
}

void	webserv::set_config(std::ifstream & config_file) {
	std::string		line;
	std::string		first_word;
	
	config_file.clear();
	config_file.seekg(0);
	while (!config_file.eof())
	{
		std::getline(config_file, line, '\n');
		int i = 0;
		while (isspace(line[i])) 
			i++;
		first_word = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
		if (first_word == "server")
			;
		else if (first_word == "client_max_body_size")
			set_max_body_size(line);
		else if (first_word == "error_page")
			set_error_page(line);	 
		else if (first_word == "}")
			;
		else if (first_word.size() != 0) {
			if (first_word[0] == 123) // ascii value for {, it fixes issue at compilation
				throw (bad_brackets_conf());
			else
				throw (bad_directive());
		}
	}
}

/* 
	client_max_body_size
	error_page
	*/
size_t	get_max_body_size(std::string & line) {
	int i = go_to_next_word(line, 0);
	std::string max_size = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
	if (!ft_string_is_digit(max_size) || (line.find_first_of(WHITESPACE, i) != line.npos) )
		throw (bad_client_max_body_size());
	return (atoi(max_size.c_str()));
}

void	webserv::set_max_body_size(std::string & line) {
	client_max_body_size = get_max_body_size(line);
	std::cout << client_max_body_size << std::endl;
}

void	webserv::set_error_page(std::string & line){
	int i = go_to_next_word(line, 0);
	std::string size = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
	std::cout << size << std::endl;
}

bool	webserv::check_brackets(const std::string & config) {
	size_t 	position = 0;
	while (1) {
		position = config.find('{', position);
		if (position == std::string::npos)
			break ;
		position = get_next_closing_bracket(config, position + 1);
		if (position == std::string::npos)
			return (false);
	}
	return (check_closing_brackets(config));
}

size_t	webserv::get_next_closing_bracket(const std::string & config, size_t position) {
	size_t next_open_bracket = config.find('{', position);
	size_t next_closing_bracket = config.find('}', position);
	
	if (next_closing_bracket == std::string::npos)
		return (std::string::npos);
	if (next_open_bracket < next_closing_bracket)
	{
		position = get_next_closing_bracket(config, next_open_bracket + 1);
		if (position == std::string::npos)
			return (std::string::npos);
		position = config.find('}', position + 1);
		if (position == std::string::npos)
			return (std::string::npos);
		else
			return (position);
	}
	return (next_closing_bracket);
}

bool webserv::check_closing_brackets(const std::string & config) {
	
	int open_count = 0;
	int close_count = 0;
	int position = 0;

	while (config.find('{', position) != std::string::npos) {
		position = config.find('{', position) + 1;
		open_count++;
	}
	position = 0;
	while (config.find('}', position) != std::string::npos) {
		position = config.find('}', position) + 1;
		close_count++;
	}
	if (close_count != open_count)
		return (false);
	else
		return (true);	
}

webserv::webserv(void) {
	
}

webserv::~webserv(void) {
	
}
