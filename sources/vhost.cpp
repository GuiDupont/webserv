/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:18:29 by ade-garr          #+#    #+#             */
/*   Updated: 2021/10/19 15:28:59 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vhost.hpp"
#include "utils.hpp"
#include "webserv.hpp"
#include "exceptions.hpp"
#include <fstream>


vHost::vHost(){

	return ;
}

vHost::vHost(std::ifstream &config_file, std::string &line) : _client_max_body_size(-1) {
	std::string	sub_line;
	std::string	first_word;
	
	g_parser.check_server_line(line);
	while (!config_file.eof())
	{
		std::getline(config_file, line, '\n');
		g_line++;
		int i = 0;
		while (std::isspace(line[i])) 
			i++;
		first_word = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
		if (first_word == "client_max_body_size") {
			this->_client_max_body_size = g_parser.get_max_body_size(line);
		}
		else if (first_word == "error_page") {
			_error_pages.insert(g_parser.parse_error_page(line));
		}
		else if (first_word == "server_name") {
			_server_name.insert(g_parser.parse_server_name(line));
		}
		else if (first_word == "listen") {
			g_parser.parse_listen(line, *this);
		}
		else if (first_word == "root")
			_root = g_parser.parse_one_word(line);
		else if (first_word == "location") {
			location loc = location(config_file, line, *this);
			if (!_locations.insert(std::pair<std::string, location>(loc.get_location_name(), loc)).second)
				throw (duplicate_location());
		}
		else if (first_word == "}")
			break ;
		else if (first_word.size() != 0) {
			if (first_word[0] == '{')
				throw (bad_brackets_conf());
			else
				throw (bad_directive());
		}
	}
	if (_host_port.empty() == 1) {
		throw (no_port_associated());
	}
	insert_map_into_map< int, std::string >(g_webserv.get_error_pages(), _error_pages);
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
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_error_pages = rhs._error_pages;
	this->_host_port = rhs._host_port;
	this->_server_name = rhs._server_name;
	this->_root = rhs._root;
	return (*this);
}

std::list< std::pair< std::string, size_t> > & vHost::getHost_Port() {
	return (this->_host_port);
}

void vHost::map_sock_to_hostport(int sock, std::pair< std::string, size_t> host_port) {

	this->_sock_list.insert(std::pair<int, std::pair< std::string, size_t> >(sock, host_port));
}

std::map< int, std::pair< std::string, size_t> > & vHost::get_sock_list() {
		return (this->_sock_list);
}

std::set<int> 					& vHost::get_csock_list() {
	return (this->_csock_list);
}

std::set<std::string> 			& vHost::get_server_names() {
	return (this->_server_name);
}

int	const						& vHost::get_client_max_body_size() const {
	return (this->_client_max_body_size);
}

std::string	const				& vHost::get_root() const {
	return (this->_root);
}

std::map< int, std::string > const	& vHost::get_error_pages() const {
	return (this->_error_pages);
}

std::map< std::string, location >	const	& vHost::get_locations() const {
	return (this->_locations);
}

