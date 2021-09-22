/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:06:41 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/22 18:38:49 by ade-garr         ###   ########.fr       */
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

request::request() : stage(0) {}

request::request(int csock) : stage(0), _csock(csock) {}

request::request(int csock, std::string left) : stage(0), _csock(csock), _left(left) {}

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
	return ;
}

bool request::is_valid_chunk_size(std::string &str) {

	regex_t	regex;
	int 	reti;

	reti = regcomp(&regex, "^[0-9a-fA-F]\\{0,8\\}$", 0);
    reti = regexec(&regex, str.c_str(), 0, NULL, 0);
	if (reti) {
    	regfree(&regex);
		return (0);
    }
    regfree(&regex);
	return (1);
}
