/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:18:29 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/08 14:57:02 by gdupont          ###   ########.fr       */
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

vHost::vHost(std::ifstream &config_file, std::string &line) {

	std::string	sub_line;
	std::string	first_word;
	

	check_server_line(line);
	while (!config_file.eof())
	{
		std::getline(config_file, line, '\n');
		int i = 0;
		while (std::isspace(line[i])) 
			i++;
		first_word = line.substr(i, line.find_first_of(" \t\n\v\f\r", i) - i);
		if (first_word == "client_max_body_size") {
			// host.
			;
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
	this->redirection = rhs.redirection;
	this->upload = rhs.upload;
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
