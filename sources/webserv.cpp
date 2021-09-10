/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 17:02:32 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/vhost.hpp"
#include <string>
#include <fstream>
#include "../includes/exceptions.hpp"
#include <iostream>


void webserv::set_hosts() {
	
	_epfd = epoll_create(1);
	std::list<vHost>::iterator it = vhosts.begin();
	for (; it != vhosts.end(); it++) {
		param_socket_server(*it);
	}
	//epoll get ready to accept

}

void	webserv::wait_for_connection() {
	struct epoll_event *revents;
	SOCKADDR_IN csin;
	socklen_t crecsize = sizeof(csin);
	revents = (struct epoll_event *)calloc(64, sizeof(*revents)); // verifier quelle valeur mettre
	char buffer[100] = "";
	struct epoll_event ev;
	while (1)
	{
		sleep(100); // a supprimer

		int nsfd = epoll_wait(this->_epfd, revents, 64, 0);
		if (nsfd)
			std::cout << nsfd << " évènements de capté(s)" << std::endl;
		else if (nsfd == -1)
			std::cout << strerror(errno) << std::endl;
		else
			std::cout << "pas dev" << std::endl;
			
		for (int i = 0; i < nsfd; i++) {
			if (revents[i].events & EPOLLIN && ft_is_ssock(revents[i].data.fd)) {
				// add new socket client to epoll
				SOCKET csock = accept(revents[i].data.fd, (SOCKADDR*)&csin, &crecsize);
				std::cout << csock << std::endl;
				std::cout << "On a accepté un client\n";
				ft_add_csock_to_vhost(revents[i].data.fd, csock);
				ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
				ev.data.fd = csock;
				if (epoll_ctl(_epfd, EPOLL_CTL_ADD, csock, &ev))
					;
			}
		}
	}
}

void	webserv::ft_add_csock_to_vhost(int sock, int csock) {
	std::list<vHost>::iterator it = this->vhosts.begin();

	for (; it != this->vhosts.end(); it++) {
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			if (sock == it_sock->first)
				it->get_csock_list().push_back(csock);	
		}
	}
}

bool	webserv::ft_is_ssock(int fd) {
	std::list<vHost>::iterator it = this->vhosts.begin();

	for (; it != this->vhosts.end(); it++) {
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			if (fd == it_sock->first)
				return (true);
		}
	}
	return (false);
}

int		webserv::get_sock_by_matching_host_ip(std::pair< std::string, size_t> host_port) {
	std::list<vHost>::iterator it = this->vhosts.begin();

	for (; it != this->vhosts.end(); it++) {
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			if (host_port == it_sock->second)
				return (it_sock->first);
		}
	}
	return (-1);
}

void	webserv::display_sock() {
	std::list<vHost>::iterator it = this->vhosts.begin();
	int i = 1;

	for (; it != this->vhosts.end(); it++) {
		std::cout << "SERVER " << i++ << std::endl;
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			std::cout << it_sock->second.first << ":" << it_sock->second.second << "is bind to sock: " << it_sock->first << std::endl;
		}
	}
}



webserv::webserv(const std::string & path_config) : auto_index(false), _client_max_body_size(0) {
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

	g_line = 0;
	config_file.clear();
	config_file.seekg(0);
	while (!config_file.eof())
	{
		std::getline(config_file, line, '\n');
		g_line++;
		int i = 0;
		while (isspace(line[i])) 
			i++;
		first_word = line.substr(i, line.find_first_of(" \t\n\v\f\r", i) - i);
		if (first_word == "server") {
			this->vhosts.push_back(vHost(config_file, line));
		}
		else if (first_word == "client_max_body_size")
			_client_max_body_size = get_max_body_size(line);
		else if (first_word == "error_page")
			_error_pages.push_back(parse_error_page(line)); 
		else if (first_word == "}")
			;
		else if (first_word.size() != 0) {
			if (first_word[0] == 123) 				// ascii value for {, it fixes issue at compilation with '{'
				throw (bad_brackets_conf());
			else
				throw (bad_directive());
		}
	}
	if (vhosts.empty() == 1)
		throw (no_port_associated()); // changer par la suite par une vraie exception pour vhost, comme recommande par Guillaume.
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

int		webserv::get_epfd() const {
	return (this->_epfd);
}

