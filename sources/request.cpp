/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:06:41 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/11 18:29:57 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"


std::ostream & operator<<(std::ostream & o,const request & r)
{
	o << "Methode: " << r.method << std::endl;
	o << "Request_target: " << r.request_target << std::endl;
	std::map<std::string, std::string>::const_iterator it = r.header_fields.begin();
	for (; it != r.header_fields.end(); it++)
	{
		o << "Header field : -" << it->first << "-" << std::endl;
		o << "Field content: -" << it->second << std::endl;
	}
	o << "Body: -" << r.body_request << "-" << std::endl;
	return (o);
}

request::~request() {
	if (body_fd != -1)
		close(body_fd);
	if (conf && conf->cgi_activated == true && cgi) {
		erase_static_fd_from_request(cgi->pipefd[0]);
		if (method == "POST")
			erase_static_fd_from_request(cgi->pipefd_post[1]);
	}
	delete conf;
	conf = NULL;
	delete resp;
	conf = NULL;
	delete cgi;
	cgi = NULL;
}

request::request() : stage(0), next_chunk(-1), nb_trailer_to_received(0), code_to_send(0), close_csock(false), conf(NULL), resp(NULL), cgi(NULL), header_is_sent(false), body_is_sent(false), body_fd(-1), body_written_cgi(0), first(false) {}

request::request(int csock) :	stage(0), csock(csock), next_chunk(-1), nb_trailer_to_received(0), 
								code_to_send(0), close_csock(false), conf(NULL), resp(NULL), cgi(NULL),
								header_is_sent(false), body_is_sent(false), body_fd(-1), body_written_cgi(0), first(false) {}

request::request(int csock, std::string left) : stage(0), csock(csock), left(left), next_chunk(-1),
												nb_trailer_to_received(0), code_to_send(0), close_csock(false),
												conf(NULL), resp(NULL), cgi(NULL), header_is_sent(false), body_is_sent(false), body_fd(-1), body_written_cgi(0), first(false) {}


void	request::send_header(int csock, std::string & header) {
	send(csock, header.c_str(), header.size(), 0);
	g_logger.fd << g_logger.get_timestamp() << "We just sent an header on csock: " << csock << std::endl;
}

void	request::send_body() {
	if (body_response.empty() == false)
		send_body_from_str();
	else if (method == "GET") //
		send_body_from_file();
	else
		body_is_sent = true;
}

void	request::send_body_from_str() {
	int amount_sent;
	int to_send;
	for (int i = 10; i != 0; i--) {
		to_send = SEND_SPEED < body_response.size() ? SEND_SPEED : body_response.size();
		amount_sent = send(csock, body_response.c_str(), to_send, 0);
		if (amount_sent == -1) {
			g_logger.fd << g_logger.get_timestamp() << "Issue while sending body on csock " << csock << ". Error: " << strerror(errno) << std::endl;
			body_is_sent = true;
			close_csock = true;
			return ;
		}
		else if (amount_sent != SEND_SPEED) {
			g_logger.fd << g_logger.get_timestamp() << "Body sending is done on csock : " << csock << std::endl;
			body_is_sent = true;
			return;
		}
		body_response = body_response.substr(to_send, body_response.size() - to_send);
	}
}

void	request::send_body_from_file() {
	int amount_read;
	int amount_sent;

	char buff[SEND_SPEED + 1];

	g_logger.fd << g_logger.get_timestamp() << "We are trying to send body from file: -" << conf->path_to_target.c_str() << std::endl;

	if (body_fd == -1) {
		body_fd = open(conf->path_to_target.c_str(), O_RDONLY);
		if (body_fd == -1) {
			g_logger.fd << g_logger.get_timestamp() << "Issue while opening -" << conf->path_to_target.c_str() << "- file on csock " << csock << ". Error: " << strerror(errno) << std::endl; // end special cases ?
			body_is_sent = true; // a check ce qu'on fait plutot envoyer une erreur 500
			close_csock = true;
			return ;
		}
		g_logger.fd << g_logger.get_timestamp() << "We opened file -" << conf->path_to_target.c_str() << "- file on fd " << body_fd << std::endl; // end special cases ?
	}
	for (int i = 4; i != 0; i--) {
		g_logger.fd << g_logger.get_timestamp() << "We are reading : " << conf->path_to_target << std::endl;
		amount_read = read(body_fd, buff, SEND_SPEED);
		buff[amount_read] = '\0';
		amount_sent = send(csock, buff, amount_read, 0);
		std::cout << " We are sending from file: " << buff << std::endl;
		if (amount_read != SEND_SPEED) { // a voir avec ALEXXXXXXXXX-----------------------------------------------------------
			body_is_sent = true;
			g_logger.fd << g_logger.get_timestamp() << "We are done sending : " << conf->path_to_target << "to csock : " << csock << std::endl;
			break;
		}
	}
}

// SIMPLE POST :

void	request::write_body_inside_file() {  // to fix !!!!
	std::ofstream file(conf->path_to_target.c_str());
	if (file.is_open() == false) {
		g_logger.fd << g_logger.get_timestamp() << "Can't open :" << conf->path_to_target.c_str();
		return ;
	}
	file << body_request;
	file.close();
	conf->local_actions_done = true;
}

void	request::erase_static_fd_from_request(int fd) {
	g_webserv.static_fds.erase(fd);
	epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_DEL, fd, NULL);
}

//////////////////////////////////////////////////* DELETE */////////////////////////////////////////////////////////////

static int		unlink_or_rmdir(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
	std::string path(fpath);
	int status;

	if (is_directory(path))
		status = rmdir(fpath);
	else
		status = unlink(fpath);
	if (status != 0)
		g_logger.fd << g_logger.get_timestamp() << "An error occured while deleting -" << path << " errno: " << errno << " " << strerror(errno) << std::endl;
	return (status);
}
				
void	request::delete_directory(std::string & path, request & req) {
	std::cout << " We are about to delete " << path << std::endl;
	nftw(path.c_str(), unlink_or_rmdir, 30, FTW_DEPTH);
	req.code_to_send = 204;
	req.body_response = response::generate_error_body(g_webserv.status_code.find(req.code_to_send)->second);
	req.conf->local_actions_done = true;
}

void	request::delete_file(std::string & path, request & req) {
	std::cout << " We are about to delete " << path << std::endl;
	int status = unlink(path.c_str());
	
	req.code_to_send = 204;
	req.body_response = response::generate_error_body(g_webserv.status_code.find(req.code_to_send)->second);
	req.conf->local_actions_done = true;
}

//////////////////////////////////////////////////* END DELETE */////////////////////////////////////////////////////////////


///////////////////////////////////////////////////// CHUNKED ///////////////////////////////////////////////////////

void request::param_trailer(std::string str) {

	std::string::iterator it = str.begin();
	std::string::iterator it2;
	std::string::iterator it3;
	std::string substr;

	if (str.size() == 0)
		return ;
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
		this->trailer.push_back(substr);
		it = (it2 == str.end() ? it2 - 1 : it2);
	}
	this->trailer.unique();
	this->nb_trailer_to_received = this->trailer.size();
	return ;
}

bool request::is_valid_chunk_size(std::string &str) {

	regex_t	regex;
	int 	reti;

	reti = regcomp(&regex, "^[0-9a-fA-F]\\{1,8\\}$", 0);
    reti = regexec(&regex, str.c_str(), 0, NULL, 0);
	if (reti) {
    	regfree(&regex);
		return (0);
    }
    regfree(&regex);
	return (1);
}

bool request::find_trailer_in_list(std::string str) {

	for (std::list<std::string>::iterator it = this->trailer.begin(); it != this->trailer.end(); it++) {
		if (str == *it)
			return (1);
	}
	return (0);
}


///////////////////////////////////////////////////// END CHUNKED ///////////////////////////////////////////////////////