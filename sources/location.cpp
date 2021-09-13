/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 17:55:12 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/12 15:23:54 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"
#include <iostream>

size_t g_line;

location::location(std::ifstream & config_file, std::string & line) : _client_max_body_size(0), _disable_methods(0), _auto_index(false) {
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
		if (first_word == "disable_methods")
			_disable_methods = g_parser.parse_disabled_methods(line);
		else if (first_word == "cgi_dir") // a finir
			;
		else if (first_word == "cgi_ext") // a finir
			g_parser.parse_cgi_extension(_cgi_ext, line);
		else if (first_word == "return")
			_return.push_back(g_parser.parse_return(line)); // only one necessary so no need for a list
		else if (first_word == "root")
			_root = g_parser.parse_one_word(line);
		else if (first_word == "upload_pass")
			_upload_pass = g_parser.parse_one_word(line);
		else if (first_word == "index")
			_index = g_parser.parse_one_word(line);
		else if (first_word == "autoindex")
			_auto_index = g_parser.parse_auto_index(line);
		else if (first_word == "error_page")
			_error_pages.push_back(g_parser.parse_error_page(line));
		else if (first_word == "client_max_body_size")
			_client_max_body_size = g_parser.get_max_body_size(line);
		else if (first_word == "}")
			return ;
		else
		{
			std::cout << "bad directive : " << first_word << std::endl;
			throw (bad_directive());
		}
	}
}

