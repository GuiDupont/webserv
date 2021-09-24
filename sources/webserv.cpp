/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:15:08 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/24 12:26:57 by gdupont          ###   ########.fr       */
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
			if (revents[i].events & EPOLLIN && ft_is_ssock(revents[i].data.fd))
				accept_new_client(revents[i].data.fd);
			else if (revents[i].events & EPOLLOUT && (!ft_is_ssock(revents[i].data.fd))) { // we have smthing to send
				g_logger.fd << g_logger.get_timestamp() << "Epoll_wait identified an EPOLLOUT on csock: " << revents[i].data.fd << std::endl;
				// config conf(g_webserv._requests.find(revents[i].data.fd)->second);
				// g_logger.fd << g_logger.get_timestamp() << conf;
				//handle_answer_to_request();
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
				read_from_csock(revents[i].data.fd);
			}
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
		std::cout << errno << strerror(errno) << std::endl; // add an exception
}

void	send_response(int csock, response * resp) {
		send(csock, resp->status_line.c_str(), resp->status_line.size(), 0);
}

void	request::response_request() {
	if (local_actions_done == false) {
		//do_local_actions(); // dlelete ou post
	}
	// if (local_actions_done == true && header_is_not_sent()) {
	// 	//generate_header();
	// 	//send_header();
	// }
	// else if (local_actions_done == true && !header_is_not_sent()) {
	// 	//send_body();
	// }
}


void	webserv::handle_answer_to_request(int csock) {
	// g_logger.fd << g_logger.get_timestamp() << "Epoll_wait identified an EPOLLOUT on csock: " << csock << std::endl;
	// request & req = g_webserv._requests.find(csock)->second;
	// //config conf(req);
	// //exit(1);
	// if (req.conf->validity_checked == false)
	// 	req.control_config_validity();
	// req.response_request();
	// if (req.done) {
	// 	//g_webserv._requests.erase(csock);
	// 	if (req.close_csock == true)
	// 		g_webserv.clean_csock_from_server(csock); // timeout
	// 	else {
	// 		struct epoll_event ev;
	// 		ev.events = EPOLLIN;
	// 		ev.data.fd = csock;
	// 		if (epoll_ctl(_epfd, EPOLL_CTL_MOD, csock, &ev) == -1)
	// 			std::cout << errno << " " << strerror(errno) << std::endl; // add an exception
	// 	}
	// }
}


void	request::control_config_validity() {
	conf->validity_checked = true;
	if (_code_to_send != 0)
		return ;
	else if (conf->_method & conf->_disable_methods && _code_to_send == 0)
		_code_to_send = 405;
	else if ((conf->_method & GET))
		//&& path_is_accessible(re))
	if (_code_to_send != 0) {
		local_actions_done = true;
		g_logger.fd << g_logger.get_timestamp() << "We are going to respond a request with code : " << _code_to_send << std::endl;
	}
}





/* REQUEST MANAGEMENT */


void webserv::analyse_header(request &req) {
	g_logger.fd << g_logger.get_timestamp() + "We are parsing header from ccosk: " << req._csock << std::endl;// analyse_body(it->second); // a faire
	if (req._left.find(std::string("\r\n\r\n"), 0) != std::string::npos) {
		int index = 0;
		req._method = get_word(req._left, index, std::string(" "));
		if (req._method != "GET" && req._method != "DELETE" && req._method != "POST")
		{
			req._code_to_send = 400;
			set_request_to_ended(req);
			req.conf = new config(req);
			return ;
		}
		req._request_target = get_word(req._left, index , std::string(" "));
		if (req._request_target.empty() || req._request_target[0] != '/' || is_valid_request_target(req._request_target) == 0)  // test nginx with charset of segment wrong https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
		{
			req._code_to_send = 400;
			set_request_to_ended(req);
			req.conf = new config(req);
			return ;
		}
		req._HTTP_version = get_word(req._left, index, std::string("\r\n"));
		if (req._HTTP_version.size() == 0)
		{
			req._code_to_send = 400;
			set_request_to_ended(req);
			req.conf = new config(req);
			return ;
		}
		if (req._HTTP_version != "HTTP/1.1" && req._HTTP_version != "HTTP/1.0")
		{
			req._code_to_send = 505;
			set_request_to_ended(req);
			req.conf = new config(req);
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
				req._code_to_send = 400;
				set_request_to_ended(req);	
				req.conf = new config(req);
				return ;
			}
			header_field = std::pair<std::string, std::string>(header_field_raw.substr(0, semi_colon_index), 
			header_field_raw.substr(semi_colon_index + 1, header_field_raw.size() - semi_colon_index));
			header_field.second = trims(header_field.second, " \t");
			if (!header_field.first.size() || !header_field.second.size() 
			|| header_field.first[header_field.first.size() - 1] == '\t'
			|| !is_token(header_field.first) || !is_field_content(header_field.second))
			{
				std::cout  << is_token(header_field.first) << is_field_content(header_field.second); // a supprimer
				req._code_to_send = 400;
				set_request_to_ended(req);
				req.conf = new config(req);
				return ;
			}
			if (req._header_fields.find(header_field.first) != req._header_fields.end())
			{
				req._code_to_send = 400;
				set_request_to_ended(req);
				req.conf = new config(req);
				return ;
			}
			req._header_fields.insert(header_field);
		}
		if (req._header_fields.find("Host") == req._header_fields.end()) {
			g_logger << "OK pb de Host sur csock : " << ft_itos(req._csock);
			req._code_to_send = 400;
			set_request_to_ended(req);
			req.conf = new config(req);
			return ;
		}
		if (req._header_fields.find("Content-Length") != req._header_fields.end()) {
			if (is_valid_content_length(req._header_fields.find("Content-Length")->second) == 0) {
				req._code_to_send = 400;
				set_request_to_ended(req);
				req.conf = new config(req);
				return ;
			}
		}
		if (req._header_fields.find("Trailer") != req._header_fields.end()) {
			req.param_trailer(req._header_fields.find("Trailer")->second);
		}
		req._left = req._left.substr(index, req._left.size() - index);
		g_logger << "POSITION DU LEFT APRES HEADER = " + req._left;
		req.stage = 1;
		req.conf = new config(req);
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
			this->_error_pages.push_back(g_parser.parse_error_page(line));
		else if (first_word == "root")
			_root = g_parser.parse_one_word(line);
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
		g_logger.fd << g_logger.get_timestamp() << "TIMEOUT, csock closed : " << *it << std::endl;// analyse_body(it->second); // a faire
		clean_csock_from_server(*it);
	}
}

void	webserv::clean_csock_from_server(int fd) {
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

webserv::webserv(void)	{ }

webserv::~webserv(void)	{ }

int					webserv::get_epfd() const { return (this->_epfd); }

std::list<vHost>	&webserv::get_vhosts() 	{ return (this->_vhosts); }

void webserv::read_from_csock(int csock) {

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

	std::string substr;
	int index = 0;

	if (req._header_fields.find("Content-Length") == req._header_fields.end() && !is_chunked(req)) {
		set_request_to_ended(req);
		if (req._left.size() != 0)
			req._left.clear();
	}
	else if (is_chunked(req)) {
		while (req._left.empty() != 1) {
			if (req.next_chunk > -1) {
				if (req._left.size() < req.next_chunk + 2)
					break;
				else {
					if (req.next_chunk == 0) {
						if (req._left[req.next_chunk] != '\r' && req._left[req.next_chunk + 1] != '\n') {
							req._code_to_send = 400;
							set_request_to_ended(req);
							return ;
						}
						req._left = req._left.substr(2, req._left.size());
						req.next_chunk = -2;
						if (req.nb_trailer_to_received == 0) {
							set_request_to_ended(req);
							return ;
						}
					}
					else {
						req._body += req._left.substr(0, req.next_chunk);
						if (req._body.size() > req.conf->_client_max_body_size) {
								req._code_to_send = 413;
								set_request_to_ended(req);
								return ;
						}
						if (req._left[req.next_chunk] != '\r' || req._left[req.next_chunk + 1] != '\n') {
							req._code_to_send = 400;
							set_request_to_ended(req);
							return ;
						}
						req._left = req._left.substr(req.next_chunk + 2, req._left.size() - (req.next_chunk + 2));
						req.next_chunk = -1;
					}
				}
			}
			else if (req.next_chunk == -1) {
				if (req._left.find("\r\n", 0) != std::string::npos) {
					substr = req._left.substr(0, req._left.find("\r\n", 0));
					if (req.is_valid_chunk_size(substr) == 0) {
						req._code_to_send = 400;
						set_request_to_ended(req);
						return ;
					}
					for (std::string::iterator it = substr.begin(); it != substr.end(); it++) {
						*it = std::tolower(*it);
					}
					req.next_chunk = ft_atoi_base(substr.c_str(), "0123456789abcdef");
					req._left = req._left.substr(req._left.find("\r\n", 0) + 2, req._left.size() - (req._left.find("\r\n", 0) + 2));
				}
				else
					break;
			}
			else {
				if (req._left.find("\r\n", 0) != std::string::npos) {
					std::pair<std::string, std::string> header_field;
					std::string header_field_raw = get_word(req._left, index, std::string("\r\n"));
					size_t semi_colon_index =  header_field_raw.find(":", 0);
					if (semi_colon_index == std::string::npos) {
						req._code_to_send = 400;
						set_request_to_ended(req);
						return ;
					}
					header_field = std::pair<std::string, std::string>(header_field_raw.substr(0, semi_colon_index), 
					header_field_raw.substr(semi_colon_index + 1, header_field_raw.size() - semi_colon_index));
					header_field.second = trims(header_field.second, " \t");
					if (!header_field.first.size() || !header_field.second.size() 
					|| header_field.first[header_field.first.size() - 1] == '\t'
					|| !is_token(header_field.first) || !is_field_content(header_field.second) || req.find_trailer_in_list(header_field.first) == 0) {
						req._code_to_send = 400;
						set_request_to_ended(req);
						return ;
					}
					req._header_fields.insert(header_field);
					req.nb_trailer_to_received = req.nb_trailer_to_received - 1;
					req._trailer.remove(header_field.first);
					req._left = req._left.substr(req._left.find("\r\n", 0) + 2, req._left.size() - (req._left.find("\r\n", 0) + 2));
					if (req.nb_trailer_to_received == 0) {
						set_request_to_ended(req);
						return ;
					}
				}
				else
					break;
			}
		}
	}
	else if (req._header_fields.find("Content-Length") != req._header_fields.end()) {
		size_t length = std::atoi(req._header_fields.find("Content-Length")->second.c_str());
		req._body += req._left;
		req._left.clear();
		if (req._body.size() > req.conf->_client_max_body_size) {
			req._code_to_send = 413;
			set_request_to_ended(req);
			return ;
		}
		if (req._body.size() >= length) {
			req._body = req._body.substr(0, length);
			set_request_to_ended(req);
			req._left.clear();
		}
	}
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

std::string		&webserv::get_root() {
	return (_root);
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
