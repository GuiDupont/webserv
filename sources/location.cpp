/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 17:55:12 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:20:31 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "location.hpp"
#include <iostream>

size_t g_line;

location::location(std::ifstream & config_file, std::string & line, vHost & host) 
		: _client_max_body_size(-1), _auto_index(false), _disable_methods(0),
		_root(host.get_root()), _cgi_dir("cgi_dir") {
	
	int i = go_to_next_word(line, 0);
	std::string path = get_word(line, i);
	i = go_to_next_word(line, i);
	std::string opening_bracket = get_word(line, i);
	if (opening_bracket != "{")
		throw (bad_location_declaration());
	_location_name = path;
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
		else if (first_word == "cgi_dir")
			_cgi_dir = g_parser.parse_one_word(line);
		else if (first_word == "cgi_ext")
			g_parser.parse_cgi_extension(_cgi_ext, line);
		else if (first_word == "return")
			_return_pair = g_parser.parse_return(line);
		else if (first_word == "root")
			_root = g_parser.parse_one_word(line);
		else if (first_word == "upload_pass")
			_upload_pass = g_parser.parse_one_word(line);
		else if (first_word == "index")
			_index = g_parser.parse_one_word(line);
		else if (first_word == "autoindex")
			_auto_index = g_parser.parse_auto_index(line);
		else if (first_word == "error_page")
			_error_pages.insert(g_parser.parse_error_page(line));
		else if (first_word == "client_max_body_size")
			_client_max_body_size = g_parser.get_max_body_size(line);
		else if (first_word == "}")
			return ;
		else
		{
			g_logger.fd  << "Bad directive : " << first_word << std::endl;
			throw (bad_directive());
		}
	}
	insert_map_into_map< int, std::string >(host.get_error_pages(), _error_pages);
}

std::string  const	& location::get_location_name() const {
	return (this->_location_name);
}

int										const		& location::get_client_max_body_size() const {
	return (this->_client_max_body_size);
}

bool									const		& location::get_auto_index() const {
	return (this->_auto_index);
}

size_t									const		& location::get_disable_methods() const {
	return (this->_disable_methods);
}

std::map< int, std::string > const					& location::get_error_pages() const {
	return (this->_error_pages);
}

std::string								const		& location::get_upload_pass() const {
	return (this->_upload_pass);
}

std::string 							const		& location::get_root() const {
	return (this->_root);
}

std::string 							const		& location::get_index() const {
	return (this->_index);
}

std::pair<int, std::string> 			const		& location::get_return() const {
	return (this->_return_pair);
}

std::set< std::string >					const		& location::get_cgi_ext() const {
	return (this->_cgi_ext);
}

std::string								const		& location::get_cgi_dir() const {
	return (this->_cgi_dir);
}
