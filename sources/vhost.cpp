/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:18:29 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/09 13:47:27 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vhost.hpp"
#include "utils.hpp"
#include "webserv.hpp"
#include "exceptions.hpp"
#include <fstream>


vHost::vHost() : port(0) {

	return ;
}

vHost::vHost(std::ifstream &config_file, std::string &line) : port(0) {

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
		if (first_word == "error_pages") {
			error_pages.push_back(parse_error_page(line)); // a retoucher selon suite
		}
		if (first_word == "return") {
			this->_return.push_back(parse_return(line));
		}
		if (first_word == "upload_pass") {
			this->upload_pass = parse_upload_pass(line);
		}
		if (first_word == "server_name") {
			this->server_name = parse_server_name(line);
		}
		if (first_word == "listen") {
			parse_listen(line, *this);
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

	this->locations = rhs.locations;
	this->auto_index = rhs.auto_index;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->allowed_methods = rhs.allowed_methods;
	this->error_pages = rhs.error_pages;
	this->_return = rhs._return;
	this->upload_pass = rhs.upload_pass;
	this->host = rhs.host;
	this->port = rhs.port;
	this->server_name = rhs.server_name;
	this->root = rhs.root;
	this->cgi = rhs.cgi;
	return (*this);
}

size_t	vHost::getPort() const {

	return (this->port);
}

void vHost::setHost(std::string host) {

	this->host = host;
	return ;
}

void vHost::setPort(int port) {

	this->port = port;
	return ;
}
