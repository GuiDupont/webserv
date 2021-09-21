/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/21 15:49:02 by ade-garr         ###   ########.fr       */
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
		control_time_out();
		int nsfd = epoll_wait(this->_epfd, revents, 64, 0);
		if (nsfd)
			g_logger << LOG_EPOLL_EVENT + ft_itos(nsfd);
		else if (nsfd == -1)
			g_logger << LOG_ISSUE_EPOLL_WAIT + std::string(strerror(errno)) << std::endl;
		else
			g_logger << "No events";
		for (int i = 0; i < nsfd; i++) {
			if (revents[i].events & EPOLLIN && ft_is_ssock(revents[i].data.fd)) { //we have a client to accept
				// add new socket client to epoll
				SOCKET csock = accept(revents[i].data.fd, (SOCKADDR*)&csin, &crecsize);
				std::time_t t = std::time(0);
				std::tm	*now = std::localtime(&t);
				_timeout.insert(std::pair<int, std::tm>(csock, *now));
				g_logger.fd << g_logger.get_timestamp() << "We accepted a new client from ssock " << revents[i].data.fd << ", new csock is = " << csock << std::endl;
				ft_add_csock_to_vhost(revents[i].data.fd, csock);
				ev.events = EPOLLIN;
				ev.data.fd = csock;
				fcntl(csock, F_SETFL, O_NONBLOCK);
				if (epoll_ctl(_epfd, EPOLL_CTL_ADD, csock, &ev) == -1)
					std::cout << errno << strerror(errno) << std::endl; // add an exception
			}
			else if (revents[i].events & EPOLLOUT && (!ft_is_ssock(revents[i].data.fd))) { // we have smthing to send
				g_logger.fd << g_logger.get_timestamp() << "Epoll_wait identified an EPOLLOUT on csock: " << revents[i].data.fd << std::endl;
				config conf(g_webserv._requests.find(revents[i].data.fd)->second);
				g_logger.fd << g_logger.get_timestamp() << conf;
				
				// a voir si eppollout direct 
				// if (is_pending_request(revents[i].data.fd))
				// 	;
				// else
				// 	handle_new_request(revents[i].data.fd);
			}
			else if (revents[i].events & EPOLLIN && (!ft_is_ssock(revents[i].data.fd))) { // we have something to read
				g_logger.fd << g_logger.get_timestamp() << "Epoll_wait identified an EPOLLIN on csock: " << revents[i].data.fd << std::endl;
				if (is_new_request(revents[i].data.fd) == 1) {
					g_logger.fd << g_logger.get_timestamp() << "New request has been created on csock: " + ft_itos(revents[i].data.fd) << std::endl;
					_requests.insert(std::pair<int, request>(revents[i].data.fd, request(revents[i].data.fd)));
				}
				add_event_to_request(revents[i].data.fd);
			}
		}
	}
}

void webserv::add_event_to_request(int csock) {

	char c_buffer[1025];
	int ret;
	std::map<int, request>::iterator it;

	ret = recv(csock, c_buffer, 1024, 0);
	if (ret < 0)
	{
		g_logger.fd << g_logger.get_timestamp() << "Error recv :" << strerror(errno) << std::endl;
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
	else if (it->second.stage == 1)
	{
		g_logger.fd << g_logger.get_timestamp() << "We start to parse body";// analyse_body(it->second); // a faire
	}
}

void webserv::analyse_header(request &req) {
	g_logger.fd << g_logger.get_timestamp() + "We are parsing header from ccosk: " << req._csock << std::endl;// analyse_body(it->second); // a faire
	if (req._left.find(std::string("\r\n\r\n"), 0) != std::string::npos) {
		int index = 0;
		req._method = get_word(req._left, index, std::string(" "));
		if (req._method != "GET" && req._method != "DELETE" && req._method != "POST")
		{
			req._error_to_send = 400;
			set_request_to_ended(req);
			return ;
		}
		req._request_target = get_word(req._left, index , std::string(" "));
		if (req._request_target.empty() || req._request_target[0] != '/' || is_valid_request_target(req._request_target) == 0)  // test nginx with charset of segment wrong https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
		{
			req._error_to_send = 400;
			set_request_to_ended(req);
			return ;
		}
		req._HTTP_version = get_word(req._left, index, std::string("\r\n"));
		if (req._HTTP_version.size() == 0)
		{
			req._error_to_send = 400;
			set_request_to_ended(req);
			return ;
		}
		if (req._HTTP_version != "HTTP/1.1" && req._HTTP_version != "HTTP/1.0")
		{
			req._error_to_send = 505;
			set_request_to_ended(req);
			return ;
		}
		while (index < req._left.size()) // parsing headerffields
		{
			std::pair<std::string, std::string> header_field;
			std::string header_field_raw = get_word(req._left, index, std::string("\r\n"));
			if (header_field_raw.size() == 0)
				break ;
			g_logger << header_field_raw;
			size_t semi_colon_index =  header_field_raw.find(":", 0);
			if (semi_colon_index == std::string::npos) {
				g_logger << "je suis rentre ici2";
				req._error_to_send = 400;
				set_request_to_ended(req);	
				return ;
			}
			header_field = std::pair<std::string, std::string>(header_field_raw.substr(0, semi_colon_index), 
			header_field_raw.substr(semi_colon_index + 1, header_field_raw.size() - semi_colon_index));
			header_field.second = trims(header_field.second, " \t");
			if (!header_field.first.size() || !header_field.second.size() 
			|| header_field.first[header_field.first.size() - 1] == '\t'
			|| !is_token(header_field.first) || !is_field_content(header_field.second))
			{
				g_logger << "je suis rentre ici";
				std::cout  << is_token(header_field.first) << is_field_content(header_field.second); // a supprimer
				req._error_to_send = 400;
				set_request_to_ended(req);
				return ;
			}
			if (req._header_fields.find(header_field.first) != req._header_fields.end())
			{
				g_logger << "je suis rentre la";
				req._error_to_send = 400;
				set_request_to_ended(req);
				return ;
			}
			req._header_fields.insert(header_field);
		}
		if (req._header_fields.find("Host") == req._header_fields.end()) {
			g_logger << "OK pb de Host sur csock : " << ft_itos(req._csock);
			req._error_to_send = 400;
			set_request_to_ended(req);
			return ;
		}
		if (req._header_fields.find("Content-Length") != req._header_fields.end() && is_chunked(req)) {
			g_logger << "Content-Length et Transfer-Encoding chunked indiques sur csock : " << ft_itos(req._csock);
			req._error_to_send = 400;
			set_request_to_ended(req);
			return ;
		}
		if (req._header_fields.find("Content-Length") != req._header_fields.end() {
			if (is_valid_content_length(req._header_fields.find("Content-Length")->second) == 0) {
				req._error_to_send = 400;
				set_request_to_ended(req);
				return ;
			}
		}
		req._left = req._left.substr(index, req._left.size() - index);
		req.stage = 1;
		analyse_body(req);
	}
}


void	webserv::set_request_to_ended(request &req) {

	struct epoll_event ev;

	req.stage = ENDED_REQUEST;
	ev.events = EPOLLOUT;
	ev.data.fd = req._csock;
	epoll_ctl(_epfd, EPOLL_CTL_MOD, req._csock, &ev);
	
	return ;
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

	g_logger.fd << g_logger.get_timestamp() << "We are receiving a new request" << std::endl;
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
				it->get_csock_list().insert(csock);	
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

webserv::webserv(const std::string & path_config) : _client_max_body_size(-1) {
	std::ifstream	config_file;
	std::string		all_file;
	
	char *tmp = getcwd(NULL, 0);
	_root = std::string(tmp) + ("/www");
	free(tmp);
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
		if (((now->tm_hour - it->second.tm_hour) * 3600 + (now->tm_min - it->second.tm_min) * 60 + (now->tm_sec - it->second.tm_sec)) >= TIMEOUT)
		{
			std::map<int, request>::iterator it_req = _requests.find(it->first);
			if (it_req == _requests.end())
				sock_to_close.push_back(it->first);
			else if (it_req->second.stage != ENDED_REQUEST)
				sock_to_close.push_back(it->first);		
		}
	}
	for (std::list<int>::iterator it = sock_to_close.begin(); it != sock_to_close.end(); it++)  {
		clean_csock_from_server(*it);
	}
}

void	webserv::clean_csock_from_server(int fd) {
	g_logger.fd << g_logger.get_timestamp() << "TIMEOUT, csock closed : " << fd << std::endl;// analyse_body(it->second); // a faire
		_requests.erase(fd);
		_timeout.erase(fd);
		for (std::list<vHost>::iterator it_vhost = _vhosts.begin(); it_vhost != _vhosts.end(); it_vhost++)
		{
			std::set<int>::iterator it_csock = it_vhost->get_csock_list().find(fd);
			if (it_csock != it_vhost->get_csock_list().end())
				it_vhost->get_csock_list().erase(it_csock);
		}
		epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
}

webserv::webserv(void) {
	
}

webserv::~webserv(void) {
	
}

int		webserv::get_epfd() const {
	return (this->_epfd);
}

std::list<vHost>				&webserv::get_vhosts() {
	return (this->_vhosts);
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
	else if (it->second.stage == 1)
		analyse_body(it->second);
}

void	webserv::analyse_body(request &req) {

	if (req._header_fields.find("Content-Length") == req._header_fields.end() && !is_chunked(req)) {
		set_request_to_ended(req);
		// g_logger << "size de left = " + ft_itos(req._left.size());
		if (req._left.size() != 0) {
			_requests.insert(std::pair<int, request>(req._csock, request(req._csock, req._left)));
			req._left.clear();
			analyse_header((--_requests.end())->second);
		}
	}
	if (req._header_fields.find("Content-Length") != req._header_fields.end()) {
		size_t length = std::atoi(req._header_fields.find("Content-Length")->second.c_str());
		req._body += req._left;
		req._left.clear();
		if (req._body.size() > length) {
			req._left = req._body.substr(length, req._body.end() - (req._body.begin() + length));
			req._body = req._body.substr(0, length);
			set_request_to_ended(req);
			_requests.insert(std::pair<int, request>(req._csock, request(req._csock, req._left)));
			req._left.clear();
			analyse_header((--_requests.end())->second);
		}
		else if (req._body.size() == length)
			set_request_to_ended(req);
	}
	// if (is_chunked(req)) {
	// 	while(req._left.empty() == 1) {
			
	// 	}
	// }
}

void	webserv::set_request_to_ended(request &req) {

	struct epoll_event ev;

	req.stage = 2;
	ev.events = EPOLLOUT;
	ev.data.fd = req._csock;
	epoll_ctl(_epfd, EPOLL_CTL_MOD, req._csock, &ev);
	return ;
}

bool webserv::is_chunked(request &req) {

	std::map<std::string, std::string>::iterator it = req._header_fields.find("Transfer-Encoding");
	if (it == req._header_fields.end())
		return (0);
	int index = find_word(it->second, "chunked");
	if (index >= 0)
		return (1);
	else
		return (0);
}

int webserv::find_word(std::string str, std::string word) {

	std::string::iterator it = str.begin();
	std::string::iterator it2;
	std::string::iterator it3;
	std::string substr;

	if (str.size() == 0 || word.size() == 0)
		return (-1);
	for (; it != str.end(); it++) {
		for (; it != str.end() && std::isspace(*it) != 0; it++) {}
		if (str.find(',', it - str.begin()) != std::string::npos)
			it2 = str.begin() + str.find(',', it - str.begin());
		else
			it2 = str.end();
		substr = str.substr(it - str.begin(), it2 - it);
		it3 = it2 - 1;
		for (; it3 - it >= 0 && std::isspace(*it3) != 0; it3--) {}
		substr = substr.substr(0, it3 - it + 1);
		if (substr == word)
			return (it - str.begin());
		it = (it2 == str.end() ? it2 - 1 : it2);
	}
	return (-1);
}

bool	webserv::is_valid_content_length(std::string val) {

    regex_t	regex;
	int 	reti;

	reti = regcomp(&regex, "^[0-9]\\{0,10\\}$", 0);
    reti = regexec(&regex, val.c_str(), 0, NULL, 0);
	if (reti) {
    	regfree(&regex);
		return (0);
    }
    regfree(&regex);
	return (1);
}
