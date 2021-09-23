/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/20 16:46:46 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/23 18:37:52 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"


config::config(void) :  validity_checked(true) { }

config::config(request & request) : validity_checked(false) {
	if (request._method == "GET")
		_method = GET;
	else if (request._method == "POST")
		_method = POST;
	else if (request._method == "DELETE")
		_method = DELETE;
	_client_max_body_size = (g_webserv._client_max_body_size  == -1 ? 0: g_webserv._client_max_body_size);
	_error_pages = g_webserv._error_pages;
	_upload_pass = g_webserv._upload_pass;
	_root = g_webserv._root;
	_cgi_dir = g_webserv._cgi_dir;
	_request_target = request._request_target;
	code = request._code_to_send;

	int first = 1;
	vHost chosen = get_associated_vhost(request);
	put_vhost_and_location_in_config(chosen, request);
	_client_max_body_size *= 1000000;
	
	g_logger.fd << g_logger.get_timestamp() << *this << std::endl;
}

void	config::put_vhost_and_location_in_config(vHost & host, request & request) {
	if (host.get_client_max_body_size() != -1)
		_client_max_body_size = host.get_client_max_body_size();
	if (!host.get_error_pages().empty())
		_error_pages = host.get_error_pages();
	if (!host.get_root().empty())
		_root = host.get_root();	
	if (!_server_name.size())
		_server_name = *host.get_server_names().begin();
	std::map< std::string, location >::const_iterator it = get_most_accurate_location(host);
	if (it != host.get_locations().end()) {
		if (it->second.get_client_max_body_size() != -1)
			_client_max_body_size = it->second.get_client_max_body_size();
		_auto_index = it->second.get_auto_index();
		_disable_methods = it->second.get_disable_methods(); 
		if (!it->second.get_error_pages().empty())
			_error_pages = it->second.get_error_pages();
		if (!it->second.get_upload_pass().empty())
			_upload_pass = it->second.get_upload_pass();
		else
			_upload_pass = _root;
		_location = it->second.get_location();
		if (!it->second.get_root().empty())
			_root = it->second.get_root();
		_index = it->second.get_index();
		path_to_target = update_path_to_target_with_root(it->second);
		_return = it->second.get_return();
		_cgi_ext = it->second.get_cgi_ext();
		return ;
	}
}

std::string		config::update_path_to_target_with_root(const location & location) {
	if (_root.empty())
		return ("");

	std::string path_to_target; 
	int index_after_location = _request_target.find(_location, 0) + _location.size();
	std::string end_of_path = _request_target.substr(index_after_location,_request_target.size() - index_after_location);
	if (_method & GET) {
		path_to_target = from_two_str_to_path(_root, end_of_path);
		if (is_directory(path_to_target))
			path_to_target = from_two_str_to_path(path_to_target, _index);
	}
	else if (_method & POST) {
		path_to_target = from_two_str_to_path(_root, _upload_pass);
		path_to_target = from_two_str_to_path(path_to_target, end_of_path);
	}
	else if (_method & DELETE)
		path_to_target = from_two_str_to_path(_root, end_of_path);
	return (path_to_target);
}

std::ostream & operator<<(std::ostream & o, const config & c)
{
	o << "HERE IS THE CONFIGURATION WE SET UP" << std::endl;
	o << "Server name: " << c._server_name << std::endl;
	o << "Location: " << c._location << std::endl;
	o << "Auto_index: " << c._auto_index << std::endl;
	o << "Client_max_body_size: " << c._client_max_body_size << std::endl;
	o << "Upload_pass: " << c._upload_pass << std::endl;
	o << "Root: " << c._root << std::endl;
	o << "CGI_dir: " << c._cgi_dir << std::endl;
	for (std::set<std::string>::const_iterator it = c._cgi_ext.begin(); it != c._cgi_ext.end(); it++)
		o << "CGI ext : " << *it << std::endl;
	o << "Index: " << c._index << std::endl;
	for (std::list<std::pair<int, std::string> >::const_iterator it = c._error_pages.begin(); it != c._error_pages.end(); it++)
		o << "Error " << it->first << " | page :" << it->second << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = c._header_fields.begin(); it != c._header_fields.end(); it++)
		o << "Header " << it->first << " | value :" << it->second << std::endl;
	o << "Return: " << c._return.first << " | Link :" << c._return.second << std::endl;
	o << "Method: " << ((c._method == GET) ? "GET" : (c._method == POST) ? "POST" : "DELETE") << std::endl;
	o << "Request_target: " << c._request_target << std::endl;
	o << "Final target: " << c.path_to_target << std::endl;
	
	return (o);
}

vHost & config::get_associated_vhost(request & request) {
	int first = 1;
	vHost *chosen;
	for (std::list<vHost>::iterator it = g_webserv._vhosts.begin(); it != g_webserv._vhosts.end(); it++) {
		if (it->get_csock_list().find(request._csock) != it->get_csock_list().end())
		{
			if (first) {
				chosen = &(*it);;
				first = 0;
			}
			if (it->get_server_names().find(request._host) 
					!= it->get_server_names().end())
			{
				_server_name = request._host;
				chosen = &(*it);
				break;
			}
		}
	}
	if (first == 1)
		g_logger.fd << g_logger.get_timestamp() << "--------We could not find any matching vhost, THAT SHOULD NOT HAPPEN---------";
	return (*chosen);
}

std::map< std::string, location >::const_iterator config::get_most_accurate_location(vHost & host) {
	std::string most_precise_location;
	std::map< std::string, location >::const_iterator it = host.get_locations().begin();
	for (; it != host.get_locations().end(); it++) {
		std::string begin_of_request_target = _request_target.substr(0, it->first.size());
		if (begin_of_request_target == it->first && begin_of_request_target.size() > most_precise_location.size()) {	
			most_precise_location = begin_of_request_target;
		}
	}
	return (host.get_locations().find(most_precise_location));		
}
