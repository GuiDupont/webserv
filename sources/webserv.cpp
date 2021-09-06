/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/06 15:46:18 by gdupont          ###   ########.fr       */
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
			return (true);
		position = get_next_bracket(config, position + 1);
		if (position == std::string::npos)
			return (false);
	}
	return (true);
}

size_t	webserv::get_next_bracket(const std::string & config, size_t position) {
	size_t next_open_bracket = config.find('{', position);
	size_t next_closing_bracket = config.find('}', position);
	
	if (next_closing_bracket == std::string::npos)
		return (std::string::npos);

	
	if (next_open_bracket < next_closing_bracket)
	{
		position = get_next_bracket(config, next_open_bracket + 1);
		position = get_next_bracket(config, position + 1);
		if (position == std::string::npos)
			return (std::string::npos);
	}
	return (next_closing_bracket);
}

webserv::webserv(void) {
	
}

webserv::~webserv(void) {
	
}
