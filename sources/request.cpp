/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:06:41 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/28 11:53:40 by gdupont          ###   ########.fr       */
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
	o << "Body: -" << r.body << "-" << std::endl;
	return (o);
}


request::request() : stage(0), next_chunk(-1), nb_trailer_to_received(0), code_to_send(0), close_csock(false), conf(NULL), resp(NULL), header_is_sent(false), body_is_sent(false), body_fd(0) {}

request::request(int csock) :	stage(0), csock(csock), next_chunk(-1), nb_trailer_to_received(0), 
								code_to_send(0), close_csock(false), conf(NULL), resp(NULL),
								header_is_sent(false), body_is_sent(false), body_fd(0) {}

request::request(int csock, std::string left) : stage(0), csock(csock), left(left), next_chunk(-1),
												nb_trailer_to_received(0), code_to_send(0), close_csock(false),
												conf(NULL), resp(NULL), header_is_sent(false), body_is_sent(false), body_fd(0) {}


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

void	request::send_header(int csock, std::string & header) {
	send(csock, header.c_str(), header.size(), 0);
	g_logger.fd << g_logger.get_timestamp() << "We just sent an header\n";
}

void	request::send_body() {
	if (body.empty() == false)
		send_body_from_str();
	else
		send_body_from_file();
}

void	request::send_body_from_str() {
	int amount_sent;
	int to_send;
	for (int i = 4; i != 0; i--) {
		to_send = SEND_SPEED < body.size() ? SEND_SPEED : body.size();
		amount_sent = send(csock, body.c_str(), to_send, 0);
		if (amount_sent == -1) {
			g_logger.fd << g_logger.get_timestamp() << "Issue while sending body on csock " << csock << ". Error: " << strerror(errno) << std::endl;
			body_is_sent = true;
			close_csock = true;
			return ;
		}
		else if (amount_sent != SEND_SPEED) {
			body_is_sent = true;
			break;
		}
		body = body.substr(to_send, body.size() - to_send);
	}
	if (body.size() == 0 || body_is_sent == true) { 
	amount_sent = send(csock, "\r\n", 2, 0);
		if (amount_sent == -1)
			g_logger.fd << g_logger.get_timestamp() << "Issue while sending body on csock " << csock << ". Error: " << strerror(errno) << std::endl;
	}
}

void	request::send_body_from_file() {
	int amount_read;
	int amount_sent;

	char buff[SEND_SPEED + 1];
	
	if (body_fd == 0)
		body_fd = open(conf->path_to_target.c_str(), O_RDONLY);
	if (body_fd == -1) {
		g_logger.fd << g_logger.get_timestamp() << "Issue while opening file on csock " << csock << ". Error: " << strerror(errno) << std::endl; // end special cases ?
		body_is_sent = true; // a check ce qu'on fait
		close_csock = true;
		return ;
	}
	g_logger.fd << g_logger.get_timestamp() << "We are sending : " << conf->path_to_target << "to csock : " << csock << std::endl;
	for (int i = 4; i != 0; i--) {
		amount_read = read(body_fd, buff, SEND_SPEED);
		buff[amount_read] = '\0';
		amount_sent = send(csock, buff, amount_read, 0);
		if (amount_read != SEND_SPEED) {
			body_is_sent = true;
			g_logger.fd << g_logger.get_timestamp() << "We are done sending : " << conf->path_to_target << "to csock : " << csock << std::endl;
			break;
		}
	}
	if (body_is_sent == true) { 
	amount_sent = send(csock, "\r\n", 2, 0);
		if (amount_sent == -1)
			g_logger.fd << g_logger.get_timestamp() << "Issue while sending body on csock " << csock << ". Error: " << strerror(errno) << std::endl;
	}
}


request::~request() {
	delete conf;
	conf = NULL;
	delete resp;
	conf = NULL;
}
