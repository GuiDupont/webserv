/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/06 17:47:04 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include <string>
#include <fstream>
#include "../includes/exceptions.hpp"
#include <iostream>


webserv::webserv(const std::string & path_config) {
	std::ifstream	config_file;
	std::string		all_file;
	
	config_file.open(path_config, std::ios_base::in);
	
	if (!config_file.is_open())
		throw (config_file_not_open());
	std::getline(config_file, all_file, '\0');
	if (this->check_brackets(all_file))
		std::cout << "config good\n";
	else
		std::cout << "config wrong\n";
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
