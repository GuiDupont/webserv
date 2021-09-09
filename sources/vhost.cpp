/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:18:29 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/09 14:40:41 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vhost.hpp"
#include "utils.hpp"
#include "webserv.hpp"
#include "exceptions.hpp"
#include <fstream>


vHost::vHost() : _port(0) {

	return ;
}

vHost::vHost(std::ifstream &config_file, std::string &line) : _port(0) {

	std::string	sub_line;
	std::string	first_word;
	
	check_server_line(line);
	while (!config_file.eof())
	{
		std::getline(config_file, line, '\n');
		g_line++;
		int i = 0;
		while (std::isspace(line[i])) 
			i++;
		first_word = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
		if (first_word == "client_max_body_size") {
			this->_client_max_body_size = get_max_body_size(line);
		}
		else if (first_word == "error_page") {
			_error_pages.push_back(parse_error_page(line)); // a retoucher selon suite
		}
		else if (first_word == "return") {
			_return.push_back(parse_return(line));
		}
		else if (first_word == "upload_pass") {
			_upload_pass = parse_upload_pass(line);
		}
		else if (first_word == "server_name") {
			_server_name = parse_server_name(line);
		}
		else if (first_word == "cgi_dir")
			;
		else if (first_word == "listen") {
			parse_listen(line, *this);
		}
		else if (first_word == "location")
			_locations.push_back(location(config_file, line));
		else if (first_word == "}")
			break ;
		else if (first_word.size() != 0) {
			if (first_word[0] == 123) 				// ascii value for {, it fixes issue at compilation with '{'
				throw (bad_brackets_conf());
			else
				throw (bad_directive());
		}
	}
	if (getPort() == 0) {
		throw (no_port_associated());
	}
}

vHost::~vHost() {

	return ;
}

vHost::vHost(vHost const & cpy) {

	*this = cpy;
	return ;
}

vHost & vHost::operator=(vHost const & rhs) {

	this->_locations = rhs._locations;
	this->_auto_index = rhs._auto_index;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_allowed_methods = rhs._allowed_methods;
	this->_error_pages = rhs._error_pages;
	this->_return = rhs._return;
	this->_upload_pass = rhs._upload_pass;
	this->_host = rhs._host;
	this->_port = rhs._port;
	this->_server_name = rhs._server_name;
	this->_root = rhs._root;
	this->_cgi = rhs._cgi;
	return (*this);
}

size_t	vHost::getPort() const {

	return (this->_port);
}

void vHost::setHost(std::string host) {

	this->_host = host;
	return ;
}

void vHost::setPort(int port) {

	this->_port = port;
	return ;
}
