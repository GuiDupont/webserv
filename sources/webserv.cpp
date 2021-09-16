/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/16 10:14:12 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void webserv::set_hosts() {
	
	_epfd = epoll_create(1);
	std::list<vHost>::iterator it = _vhosts.begin();
	for (; it != _vhosts.end(); it++) {
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
		sleep(3); // a supprimer

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
				fcntl(csock, F_SETFL, O_NONBLOCK);
				if (epoll_ctl(_epfd, EPOLL_CTL_ADD, csock, &ev) == -1)
					std::cout << errno << strerror(errno) << std::endl; // add an exception
			}
			else if (revents[i].events & EPOLLOUT && revents[i].events & EPOLLIN && !ft_is_ssock(revents[i].data.fd)) { 
				// a voir si eppollout direct 
				if (is_pending_request(revents[i].data.fd))
					;
				else
					handle_new_request(revents[i].data.fd);
			}
		}
	}
}

void	webserv::handle_new_request(int csock) {
	request 	new_request;
	int			ret;
	
	char c_buffer[20000];
	ret = recv(csock, c_buffer, 19999, 0);
	std::cout << strlen(c_buffer) << std::endl;
	c_buffer[19999] = '\0';
	std::string buff(c_buffer);
	std::cout << buff << std::endl;
	int index = buff.find(" ", 0);
	new_request._method = buff.substr(0, index);
	std::cout << "-" << new_request._method << "-" << std::endl;
	if (buff != "GET" && buff != "DELETE" && buff != "POST")
		;// BAD REQUEST
	int index2 = buff.find(" ", index + 1);
	new_request._request_target = buff.substr(index + 1, index2 - (index + 1));
	std::cout << "-" << new_request._request_target << "-" << std::endl;
	if (new_request._request_target.empty() || new_request._request_target[0] != '\\' )  // test nginx with charset of segment wrong https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
		;// BAD stuff check error to send
	index = buff.find("\r\n", index2 + 1);
	new_request._HTTP_version = buff.substr(index2 + 1, index - (index2 + 1));
	std::cout << "-" << new_request._HTTP_version << "-" << std::endl;
	if (new_request._HTTP_version != "HTTP/1.1")
		std::cout << "not HTTP/1.1\n"; // check what to so in this case;
	
		
}

bool	webserv::is_pending_request(int csock) {
	if (_pending_requests.find(csock) != _pending_requests.end())
		return (true);
	return (false);
}

void	webserv::ft_add_csock_to_vhost(int sock, int csock) {
	std::list<vHost>::iterator it = this->_vhosts.begin();

	for (; it != this->_vhosts.end(); it++) {
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			if (sock == it_sock->first)
				it->get_csock_list().push_back(csock);	
		}
	}
}

bool	webserv::ft_is_ssock(int fd) {
	std::list<vHost>::iterator it = this->_vhosts.begin();

	for (; it != this->_vhosts.end(); it++) {
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			if (fd == it_sock->first)
				return (true);
		}
	}
	return (false);
}

int		webserv::get_sock_by_matching_host_ip(std::pair< std::string, size_t> host_port) {
	std::list<vHost>::iterator it = this->_vhosts.begin();

	for (; it != this->_vhosts.end(); it++) {
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			if (host_port == it_sock->second)
				return (it_sock->first);
		}
	}
	return (-1);
}

void	webserv::display_sock() {
	std::list<vHost>::iterator it = this->_vhosts.begin();
	int i = 1;

	for (; it != this->_vhosts.end(); it++) {
		std::cout << "SERVER " << i++ << std::endl;
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			std::cout << it_sock->second.first << ":" << it_sock->second.second << "is bind to sock: " << it_sock->first << std::endl;
		}
	}
}

/* CONSTRUCTOR */

webserv::webserv(const std::string & path_config) : _auto_index(false), _client_max_body_size(0) {
	std::ifstream	config_file;
	std::string		all_file;
	
	config_file.open(path_config.c_str(), std::ios_base::in);
	if (!config_file.is_open())
		throw (config_file_not_open());
	std::getline(config_file, all_file, '\0');
	if (!g_parser.check_brackets(all_file))
		throw (bad_brackets_conf());
	set_config(config_file);
}

void		webserv::set_config(std::ifstream & config_file) {
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
			this->_vhosts.push_back(vHost(config_file, line));
		}
		else if (first_word == "client_max_body_size")
			this->_client_max_body_size = g_parser.get_max_body_size(line);
		else if (first_word == "error_page")
			this->_error_pages.push_back(g_parser.parse_error_page(line)); 
		else if (first_word == "}")
			;
		else if (first_word.size() != 0) {
			if (first_word[0] == 123) 				// ascii value for {, it fixes issue at compilation with '{'
				throw (bad_brackets_conf());
			else
				throw (bad_directive());
		}
	}
	if (this->_vhosts.empty() == 1)
		throw (no_port_associated()); // changer par la suite par une vraie exception pour vhost, comme recommande par Guillaume.
}

webserv::webserv(void) {
	
}

webserv::~webserv(void) {
	
}

int		webserv::get_epfd() const {
	return (this->_epfd);
}

