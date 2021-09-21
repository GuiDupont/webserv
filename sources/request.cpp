/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:06:41 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/21 13:49:18 by ade-garr         ###   ########.fr       */
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

request::request(std::string & header) { // a supprimer ?? (ancienne fonction pour avoir le bdy)
	int index = 0;
	_method = get_word(header, index, std::string(" "));
	if (_method != "GET" && _method != "DELETE" && _method != "POST")
	{
		_error_to_send = 400;
		return ;
	}
	_request_target = get_word(header, index , std::string(" "));
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

request::request() : stage(0) {}

request::request(int csock) : stage(0), _csock(csock) {}

request::request(int csock, std::string left) : stage(0), _csock(csock), _left(left) {}
