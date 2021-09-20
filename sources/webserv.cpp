/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/20 12:38:29 by ade-garr         ###   ########.fr       */
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
		if (nsfd) {
			std::cout << nsfd << " évènements de capté(s)" << std::endl;
		}
		else if (nsfd == -1)
			std::cout << strerror(errno) << std::endl;
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
				std::time_t t = std::time(0);
				std::tm	*now = std::localtime(&t);
				_timeout.insert(std::pair<int, std::tm>(csock, *now));
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
				if (is_new_request(revents[i].data.fd) == 1)
					_requests.insert(std::pair<int, request>(revents[i].data.fd, request()));
				add_event_to_request(revents[i].data.fd);
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

void	webserv::handle_new_request(int csock) { // a supprimer ?? (ancienne fonction pour avoir le bdy)
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

void webserv::add_event_to_request(int csock) {

	char c_buffer[1025];
	int ret;
	std::map<int, request>::iterator it;

	ret = recv(csock, c_buffer, 1024, 0);
	if (ret < 0)
	{
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	std::time_t t = std::time(0);
	_timeout.find(csock)->second = *std::localtime(&t);
	c_buffer[ret] = '\0';
	for (it = _requests.begin(); it != _requests.end(); it++) {
		if (it->first == csock && it->second.stage != ENDED_REQUEST)
			break ;
	}
	it->second._left += c_buffer;
	if (it->second.stage == 0)
		analyse_header(it->second);
	else (it->second.stage == 1)
		analyse_body(it->second);
}

void webserv::analyse_header(request &req) {

	if (req._left.find(std::string("\r\n"), 0) != std::npos) {
		int index = 0;
		req._method = get_word(req._left, index, std::string(" "));
		if (req._method != "GET" && req._method != "DELETE" && req._method != "POST")
		{
			req._error_to_send = 400;
			return ;
		}
		req._request_target = get_word(req._left, index , std::string(" "));
		if (_request_target.empty() || _request_target[0] != '/' )  // test nginx with charset of segment wrong https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
		{
			_error_to_send = 400;
			return ;
		}
		_HTTP_version = get_word(header, index, std::string("\r\n"));
		if (_HTTP_version.size() == 0)
		{
			_error_to_send = 400;	
			return ;
		}
		if (_HTTP_version != "HTTP/1.1" && _HTTP_version != "HTTP/1.0")
			_error_to_send = 505;
		while (index < header.size()) // parsing headerffields
		{
			std::pair<std::string, std::string> header_field;
			std::string header_field_raw = get_word(header, index, std::string("\r\n"));
			if (header_field_raw.size() == 0)
				return ;
			std::cout << header_field_raw << std::endl;
			int semi_colon_index =  header_field_raw.find(":", 0);
			if (semi_colon_index != std::string::npos)
				_error_to_send = 1; // seet good error
			header_field = std::pair<std::string, std::string>(header_field_raw.substr(0, semi_colon_index), 
			header_field_raw.substr(semi_colon_index + 1, header_field_raw.size() - semi_colon_index));
			header_field.second = trims(header_field.second, " \t");
			
			if (!header_field.first.size() || !header_field.second.size() 
			|| header_field.first[header_field.first.size() - 1] == '\t'
			|| !is_token(header_field.first) || !is_field_content(header_field.second))
			{
				std::cout  << is_token(header_field.first) << is_field_content(header_field.second);
				_error_to_send = 400;
				return ;
			}
			if (_header_fields.find(header_field.first) != _header_fields.end())
			{
				_error_to_send = 400;
				return ;
			}
			_header_fields.insert(header_field);
		}
		if (_header_fields.find("Host") == _header_fields.end()) {
			std::cout << "OK pb de Host" << std::endl;
			_error_to_send = 400;
			return ;
		}
	}
}
