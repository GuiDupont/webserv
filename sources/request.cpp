/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:06:41 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/23 18:20:24 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"


std::ostream & operator<<(std::ostream & o,const request & r)
{
	o << "Methode: " << r._method << std::endl;
	o << "Request_target: " << r._request_target << std::endl;
	std::map<std::string, std::string>::const_iterator it = r._header_fields.begin();
	for (; it != r._header_fields.end(); it++)
	{
		std::cout << "Header field : -" << it->first << "-" << std::endl;
		std::cout << "Field content: -" << it->second << std::endl;
	}
	std::cout << "Body: -" << r._body << "-" << std::endl;
	return (o);
}

request::request() : stage(0), next_chunk(-1), nb_trailer_to_received(0), _code_to_send(0), close_csock(false), local_actions_done(false), conf(NULL), resp(NULL) {}

request::request(int csock) : stage(0), _csock(csock), next_chunk(-1), nb_trailer_to_received(0), _code_to_send(0), close_csock(false), local_actions_done(false), conf(NULL), resp(NULL){}

request::request(int csock, std::string left) : stage(0), _csock(csock), _left(left), next_chunk(-1), nb_trailer_to_received(0), _code_to_send(0), close_csock(false), local_actions_done(false), conf(NULL), resp(NULL) {}

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
		this->_trailer.push_back(substr);
		it = (it2 == str.end() ? it2 - 1 : it2);
	}
	this->_trailer.unique();
	this->nb_trailer_to_received = this->_trailer.size();
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

	for (std::list<std::string>::iterator it = this->_trailer.begin(); it != this->_trailer.end(); it++) {
		if (str == *it)
			return (1);
	}
	return (0);
}

request::~request() {
	delete conf;
	delete resp;
}
