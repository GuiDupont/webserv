/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/28 18:47:48 by gdupont          ###   ########.fr       */
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
	time_t t = time(0);
	struct epoll_event *revents;
	SOCKADDR_IN csin;
	socklen_t crecsize = sizeof(csin);
	revents = (struct epoll_event *)calloc(64, sizeof(*revents)); // verifier quelle valeur mettre
	char buffer[100] = "";
	struct epoll_event ev;
	
	while (1)
	{
		control_time_out();
		int nsfd = epoll_wait(this->_epfd, revents, 64, 0);
		if (nsfd)
			g_logger << LOG_EPOLL_EVENT + ft_itos(nsfd);
		else if (nsfd == -1)
			g_logger << LOG_ISSUE_EPOLL_WAIT + std::string(strerror(errno)) << std::endl;
		else if (true_one_time_per_x_secondes(5))
			g_logger << "No events";

		for (int i = 0; i < nsfd; i++) {
			if (revents[i].events & EPOLLIN && ft_is_ssock(revents[i].data.fd) && _stop == false)
				accept_new_client(revents[i].data.fd);
			else if (revents[i].events & EPOLLIN && (!ft_is_ssock(revents[i].data.fd)) && _stop == false) // we have something to read
				read_from_csock(revents[i].data.fd);
			else if (revents[i].events & EPOLLOUT && (!ft_is_ssock(revents[i].data.fd)))
				answer_to_request(revents[i].data.fd);
		}
	}
}

void	webserv::accept_new_client(int sock) {
	struct epoll_event ev;
	SOCKADDR_IN csin;
	socklen_t crecsize = sizeof(csin);
	SOCKET csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
	std::time_t t = std::time(0);
	std::tm	*now = std::localtime(&t);
	_timeout.insert(std::pair<int, std::tm>(csock, *now));
	g_logger.fd << g_logger.get_timestamp() << "We accepted a new client from ssock " << sock << ", new csock is = " << csock << std::endl;
	ft_add_csock_to_vhost(sock, csock);
	ev.events = EPOLLIN;
	ev.data.fd = csock;
	fcntl(csock, F_SETFL, O_NONBLOCK);
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, csock, &ev) == -1)
		g_logger.fd << g_logger.get_timestamp() << "We had an issue with EPOLL CTL, trying to add a csock to the pool. errno :" << errno << ": " << strerror(errno) << std::endl; // add an exception
}

void	webserv::answer_to_request(int csock) {
	g_logger.fd << g_logger.get_timestamp() << "Epoll_wait identified an EPOLLOUT on csock: " << csock << std::endl;

	request & req = g_webserv._requests.find(csock)->second;
	if (req.conf->validity_checked == false) {
		req.control_config_validity();
		req.update_code_and_body();
	}
	req.response_request();
	if (req.body_is_sent == true) {
		if (req.close_csock == true)
			g_webserv.clean_csock_from_server(csock); // timeout
		else {
			struct epoll_event ev;
			ev.events = EPOLLIN;
			ev.data.fd = csock;
			if (epoll_ctl(_epfd, EPOLL_CTL_MOD, csock, &ev) == -1)
				g_logger.fd << g_logger.get_timestamp() << " " << errno << " " << strerror(errno) << std::endl; // add an exception
		}
	}
}

void	request::control_config_validity() {
	conf->validity_checked = true;
	if (code_to_send != 0)
		return ;
	else if (common_validity_check() == false)
		return ;
	else if (conf->method & GET) {
		test_path_get(*this);
	}
	else if (conf->method & DELETE) {
		test_path_delete(*this);
	}
	else if (conf->method & POST)
		test_path_post(*this);
}

void	request::update_code_and_body() {
	if (code_to_send != 0) {
		conf->local_actions_done = true;
		close_csock = true;
		std::map< int, std::string>::iterator it;
		if ((it = conf->_error_pages.find(code_to_send)) != conf->_error_pages.end()) {
			if (is_valid_file(it->second)) {
				conf->path_to_target = it->second;	
				return ;
			}
		}
		body = response::generate_error_body(g_webserv.status_code.find(code_to_send)->second);
	}
	// else 
	// 	code_to_send = 200;
}

bool	request::common_validity_check() {
	if (conf->method & conf->_disable_methods) {
		code_to_send = 405;
		return (false);
	}
	else if (conf->_return.second.empty() == false) {
		code_to_send = conf->_return.first;
		conf->return_activated = true;
		return (false);
	}
	
	return (true);
}

void	request::response_request() {
	if (conf->local_actions_done == false)
		do_local_actions();
	if (conf->local_actions_done == true && header_is_sent == false) {
		g_logger.fd << g_logger.get_timestamp() << "We are going to respond a request with code : " << code_to_send << std::endl;
	 	std::string header = response::generate_header(*this);
	 	send_header(csock, header);
		header_is_sent = true;
	}
	if (conf->local_actions_done == true && header_is_sent == true) {
		send_body();
	}
}

void	request::do_local_actions() {
	if (conf->method & GET) {
		conf->local_actions_done = true;
		code_to_send = 200;
	}
	else if (conf->method & DELETE) {
		if (is_directory(conf->path_to_target))
			delete_directory(conf->path_to_target, *this);
		else
			delete_file(conf->path_to_target, *this);
		code_to_send = 204;
		
	}
	else if (conf->method & POST)
		conf->local_actions_done = true;
}
// unlink
// rmdir
/* REQUEST MANAGEMENT */


void	request::set_request_to_ended() {

	struct epoll_event ev;

	stage = ENDED_REQUEST;
	ev.events = EPOLLOUT;
	ev.data.fd = csock;
	epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_MOD, csock, &ev); // check return
}

void	webserv::set_request_to_ended(request &req) {

	struct epoll_event ev;

	req.stage = ENDED_REQUEST;
	ev.events = EPOLLOUT;
	ev.data.fd = req.csock;
	epoll_ctl(_epfd, EPOLL_CTL_MOD, req.csock, &ev);
}

bool webserv::is_new_request(int fd) {

	for (std::map<int, request>::iterator it = _requests.begin(); it != _requests.end(); it++) {
		if (it->first == fd) {
			if (it->second.stage != ENDED_REQUEST)
				return (false);
		}
	}
	// if (_requests.find(fd) != _requests.end())
	// 	return (true);
	return (true);
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
		g_logger.fd << g_logger.get_timestamp() << "SERVER " << i++ << std::endl;
		std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin();
		for (; it_sock != it->get_sock_list().end(); it_sock++) {
			g_logger.fd << g_logger.get_timestamp() << it_sock->second.first << ":" << it_sock->second.second << "is bind to sock: " << it_sock->first << std::endl;
		}
	}
}

webserv::webserv(const std::string & path_config) : _client_max_body_size(-1) {
	std::ifstream	config_file;
	std::string		all_file;
	
	config_file.open(path_config.c_str(), std::ios_base::in);
	if (!config_file.is_open())
		throw (config_file_not_open());
	std::getline(config_file, all_file, '\0');
	if (!g_parser.check_brackets(all_file))
		throw (bad_brackets_conf());
	set_config(config_file);
	insert_status_code();
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
			this->_error_pages.insert(g_parser.parse_error_page(line));
		else if (first_word == "root")
			_root = g_parser.parse_one_word(line);
		else if (first_word == "}")
			;
		else if (first_word.size() != 0) {
			if (first_word[0] == '{') 	// 123 : ascii value for {, it fixes issue at compilation with '{'
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
		g_logger.fd << g_logger.get_timestamp() << "TIMEOUT, we closed csock : " << *it << std::endl;// analyse_body(it->second); // a faire
		clean_csock_from_server(*it);
	}
}

void	webserv::clean_csock_from_server(int csock) {
		_requests.erase(csock);
		_timeout.erase(csock);
		for (std::list<vHost>::iterator it_vhost = _vhosts.begin(); it_vhost != _vhosts.end(); it_vhost++)
		{
			std::set<int>::iterator it_csock = it_vhost->get_csock_list().find(csock);
			if (it_csock != it_vhost->get_csock_list().end())
				it_vhost->get_csock_list().erase(it_csock);
		}
		epoll_ctl(_epfd, EPOLL_CTL_DEL, csock, NULL);
		close(csock);
}

webserv::webserv(void)	{ }

webserv::~webserv(void)	{ }

int					webserv::get_epfd() const { return (this->_epfd); }

std::list<vHost>	&webserv::get_vhosts() 	{ return (this->_vhosts); }

void webserv::read_from_csock(int csock) {
	g_logger.fd << g_logger.get_timestamp() << "Epoll_wait identified an EPOLLIN on csock: " << csock << std::endl;
	char c_buffer[1025];
	int ret;
	std::map<int, request>::iterator it;

	if (is_new_request(csock) == true) {
		g_logger.fd << g_logger.get_timestamp() << "New request has been created from csock: " + ft_itos(csock) << std::endl;
		_requests.insert(std::pair<int, request>(csock, request(csock)));
	}
	ret = recv(csock, c_buffer, 1024, 0);
	if (ret < 0) {
		g_logger.fd << "We had an issue while using recv on csock " << csock << " " << strerror(errno) << std::endl;
		clean_csock_from_server(csock);
		return ;
	}
	if (ret == 0) {
		g_logger.fd << "While using recv on csock " << csock << " we received : 0" <<  std::endl;
		clean_csock_from_server(csock);
		return ;
	}
	std::time_t t = std::time(0);
	_timeout.find(csock)->second = *std::localtime(&t);
	c_buffer[ret] = '\0';
	for (it = _requests.begin(); it != _requests.end(); it++) {
		if (it->first == csock)
			break ;
	}
	it = _requests.find(csock);   //&& it->second.stage != ENDED_REQUEST
	if (it == _requests.end()) {
		g_logger.fd << g_logger.get_timestamp() << "We can't match " << csock << " to any VHOST" << " that SHOULD NOT HAPPEN" << std::endl;
		return ;
	}
	it->second.left += c_buffer;
	if (it->second.stage == 0)
		g_parser.analyse_header(it->second);
	else if (it->second.stage == 1)
		g_parser.analyse_body(it->second);
}

std::string		&webserv::get_root() { return (_root); }

const bool		&webserv::get_stop() const { return (_stop); }

void			webserv::set_stop(bool value) { _stop = value; }

std::map< int, std::string > const	& webserv::get_error_pages() const {
	return (this->_error_pages);
}

void	webserv::insert_status_code() {

	status_code.insert(std::make_pair(100, "100 Continue"));
	status_code.insert(std::make_pair(101, "101 Switching Protocols"));

	status_code.insert(std::make_pair(200, "200 OK"));
	status_code.insert(std::make_pair(201, "201 Created"));
	status_code.insert(std::make_pair(202, "202 Accepted"));
	status_code.insert(std::make_pair(203, "203 Non-Authoritative Information"));
	status_code.insert(std::make_pair(204, "204 No Content"));
	status_code.insert(std::make_pair(205, "205 Reset Content"));
	status_code.insert(std::make_pair(206, "206 Partial Content"));

	status_code.insert(std::make_pair(300, "300 Multiple Choices"));
	status_code.insert(std::make_pair(301, "301 Moved Permanently"));
	status_code.insert(std::make_pair(302, "302 Found"));
	status_code.insert(std::make_pair(303, "303 See Other"));
	status_code.insert(std::make_pair(304, "304 Not Modified"));
	status_code.insert(std::make_pair(305, "305 Use Proxy"));

	status_code.insert(std::make_pair(307, "307 Temporary Redirect"));

	status_code.insert(std::make_pair(400, "400 Bad Request"));
	status_code.insert(std::make_pair(401, "401 Unauthorized"));
	status_code.insert(std::make_pair(402, "402 Payment Required"));
	status_code.insert(std::make_pair(403, "403 Forbidden"));
	status_code.insert(std::make_pair(404, "404 Not Found"));
	status_code.insert(std::make_pair(405, "405 Method Not Allowed"));
	status_code.insert(std::make_pair(406, "406 Not Acceptable"));
	status_code.insert(std::make_pair(407, "407 Proxy Authentication Required"));
	status_code.insert(std::make_pair(408, "408 Request Timeout"));
	status_code.insert(std::make_pair(409, "409 Conflict"));
	status_code.insert(std::make_pair(410, "410 Gone"));
	status_code.insert(std::make_pair(411, "411 Length Required"));
	status_code.insert(std::make_pair(412, "412 Precondition Failed"));
	status_code.insert(std::make_pair(413, "413 Payload Too Large"));
	status_code.insert(std::make_pair(414, "414 URI Too Long"));
	status_code.insert(std::make_pair(415, "415 Unsupported Media Type"));
	status_code.insert(std::make_pair(416, "416 Range Not Satisfiable"));
	status_code.insert(std::make_pair(417, "417 Expectation Failed"));

	status_code.insert(std::make_pair(426, "426 Upgrade Required"));

	status_code.insert(std::make_pair(500, "500 Internal Server Error"));
	status_code.insert(std::make_pair(501, "501 Not Implemented"));
	status_code.insert(std::make_pair(502, "502 Bad Gateway"));
	status_code.insert(std::make_pair(503, "503 Service Unavailable"));
	status_code.insert(std::make_pair(504, "504 Gateway Timeout"));
	status_code.insert(std::make_pair(505, "505 HTTP Version Not Supported"));
}
