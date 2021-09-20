/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/20 14:11:25 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void webserv::set_hosts() {
	
	_epfd = epoll_create(1);
	g_logger << LOG_EPOLL_CREATE + ft_itos(_epfd);
	std::list<vHost>::iterator it = _vhosts.begin();
	for (; it != _vhosts.end(); it++) {
		param_socket_server(*it);
	}
	//epoll get ready to accept
}

void	webserv::wait_for_connection() {
	g_logger << LOG_WAIT_CO;
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
		{
			std::cout << nsfd << " évènements de capté(s)" << std::endl;
			g_logger << LOG_EPOLL_EVENT + ft_itos(nsfd);
		}
		else if (nsfd == -1)
			g_logger << LOG_ISSUE_EPOLL_WAIT + std::string(strerror(errno)) << std::endl;
		else
			std::cout << "Pas d'événement" << std::endl;
		for (int i = 0; i < nsfd; i++) {
			// if (revents[i].events & EPOLLIN)
			// 	std::cout << "EPOLLIN on " << (ft_is_ssock(revents[i].data.fd) ? "ssock" : "csock") << std::endl;			
			// if (revents[i].events & EPOLLOUT)
			// 	std::cout << "EPOLLOUT on " << (ft_is_ssock(revents[i].data.fd) ? "ssock" : "csock") << std::endl;
			if (revents[i].events & EPOLLIN && ft_is_ssock(revents[i].data.fd)) { //we have a client to accept
				// add new socket client to epoll
				SOCKET csock = accept(revents[i].data.fd, (SOCKADDR*)&csin, &crecsize);
				std::cout << csock << std::endl;
				std::cout << "On a accepté un client\n";
				ft_add_csock_to_vhost(revents[i].data.fd, csock);
				ev.events = EPOLLIN | EPOLLOUT;
				ev.data.fd = csock;
				fcntl(csock, F_SETFL, O_NONBLOCK);
				if (epoll_ctl(_epfd, EPOLL_CTL_ADD, csock, &ev) == -1)
					std::cout << errno << strerror(errno) << std::endl; // add an exception
			}
			else if (revents[i].events & EPOLLOUT && (!ft_is_ssock(revents[i].data.fd))) { // we have smthing to send
				// a voir si eppollout direct 
				// if (is_pending_request(revents[i].data.fd))
				// 	;
				// else
				// 	handle_new_request(revents[i].data.fd);
			}
			else if (revents[i].events & EPOLLIN && (!ft_is_ssock(revents[i].data.fd))) { // we have something to read
				if (is_new_request(revents[i].data.fd) == 1) {
					// essayer de reto;ber sur la ft handle_new_request;
				}
				else {
					// a completer
				}
			}
		}
	}
}

bool webserv::is_new_request(int fd) {

	for (std::map<int, request>::iterator it = _requests.begin(); it != _requests.end(); it++) {
		if (it->first == fd) {
			if (it->second.stage != ENDED_REQUEST)
				return (0);
		}
	}
	return (1);
}

void	webserv::handle_new_request(int csock) {
	int			ret;
	std::pair<std::string, std::string> header_body;
	int index = 0;

	std::cout << " We are in new request\n";
	header_body = g_parser.get_header_begin_body(csock);
	request 	new_request(header_body.first); // a optimiser avec constructeur de pair
	new_request._body = header_body.second;
	std::cout << new_request;

	// parse msg body
}

bool	webserv::is_pending_request(int csock) {
	if (_requests.find(csock) != _requests.end())
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
	g_logger << LOG_CONFIG_DONE;
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

void	webserv::control_time_out(void) {
	
	std::time_t t = std::time(0);
	std::tm*   now = std::localtime(&t);
	std::list<int>	sock_to_close;

	
	for (std::map<int, std::tm>::iterator it = _timeout.begin(); it != _timeout.end(); it++) {
		
		if ((now->tm_hour - it->second.tm_hour) != 0 || 
			(now->tm_min - it->second.tm_min) != 0 || 
			(now->tm_sec - it->second.tm_sec) > 20)
		{
			std::map<int, request>::iterator it_req = _requests.find(it->first);
			if (it_req == _requests.end())
				sock_to_close.push_back(it->first);
			else if (it_req->second.stage != ENDED_REQUEST)
				sock_to_close.push_back(it->first);		
		}
	}
	for (std::list<int>::iterator it = sock_to_close.begin(); it != sock_to_close.end(); it++)  {
		close(*it);
		_requests.erase(*it);
		_timeout.erase(*it);
		epoll_ctl(_epfd, EPOLL_CTL_DEL, *it, NULL);
	}
}

webserv::webserv(void) {
	
}

webserv::~webserv(void) {
	
}

int		webserv::get_epfd() const {
	return (this->_epfd);
}

