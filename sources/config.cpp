/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/20 16:46:46 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/06 15:56:35 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"


config::config(void) :  validity_checked(true), return_activated(false), local_actions_done(false), cgi_activated(false) { }

config::config(request & request) : validity_checked(false), return_activated(false), local_actions_done(false), cgi_activated(false) {
	if (request.method == "GET")
		method = GET;
	else if (request.method == "POST")
		method = POST;
	else if (request.method == "DELETE")
		method = DELETE;
	client_max_body_size = (g_webserv._client_max_body_size  == -1 ? 0: g_webserv._client_max_body_size);
	_error_pages = g_webserv._error_pages;
	_upload_pass = g_webserv._upload_pass;
	_root = g_webserv._root;
	_cgi_dir = g_webserv._cgi_dir;
	_request_target = request.request_target;
	code = request.code_to_send;

	int first = 1;
	vHost chosen = get_associated_vhost(request);
	put_vhost_and_location_in_config(chosen, request);
	client_max_body_size *= 1000000;
	set_cgi_params(request);
	//g_logger.fd
	std::cout << g_logger.get_timestamp() << *this << std::endl;
	
}

static size_t get_cgi_ext_pos(const std::string & target) {
	size_t start_search = 0;
	size_t start_cgi_ext = 0;

	while (1) {
		start_cgi_ext = target.find(CGI_EXT, start_search);
		if (start_cgi_ext == std::string::npos)
			return (start_cgi_ext);
		else
		{
			start_search = start_cgi_ext + strlen(CGI_EXT);
			std::string candidate = target.substr(0, start_search);
			if (is_file(candidate) || is_symlink(candidate))
				return (start_search);
		}
	}
	return (std::string::npos);
}

size_t	config::get_query_index(const std::string & target, size_t index) {
	if (index == std::string::npos)
		index = 0;
	size_t index_query = target.find_first_of("?", index);
	if (index_query == std::string::npos)
		return (target.size() - 1);
	return (index_query);
}

void	config::set_cgi_params(request & req) {

	std::string &target = path_to_target;
	size_t begin_cgi_ext = 0;
	int cgi_activated = 0;
	size_t first_diez = target.find_first_of("#", 0);
	if (first_diez != std::string::npos && first_diez != 0)
		target = target.substr(0, first_diez - 1);
	begin_cgi_ext = get_cgi_ext_pos(target);
	size_t query_index = get_query_index(target, begin_cgi_ext);
	query_string = target.substr(query_index, target.size() - query_index);
	target = target.substr(0, query_index);
	size_t path_info_index = 0;
	if (begin_cgi_ext == std::string::npos)
		path_info_index = req.conf->_root.size();
	else
		path_info_index = begin_cgi_ext + strlen(CGI_EXT);	
	path_info = target.substr(path_info_index, target.size() - path_info_index);
	target = target.substr(0, path_info_index);

	// if (begin_cgi_ext == std::string::npos)
	// 	return (no_cgi_ext(req, path));
	


	// if (cgi_ext.find(CGI_EXT) != cgi_ext.end())
	// 	cgi_activated = true;
	// size_t first_query = target.find_first_of("?", begin_cgi_ext);
	// if (first_query != std::string::npos) {
	// 	query_string = target.substr(first_query + 1, target.length() - (first_query + 1));
	// 	if (first_query != 0)
	// 		target = target.substr(0, first_query);
	// }
	// // else
	// if (first_query - begin_cgi_ext >= 2)
	// 	path_info = target.substr(begin_cgi_ext, first_query);
	// if (begin_cgi_ext != 0)
	// 	target = target.substr(0, begin_cgi_ext);
	// if (begin_cgi_ext != std::string::npos)
	// 	path_info += req.conf->_root;
}

void	config::put_vhost_and_location_in_config(vHost & host, request & request) {
	host_port = *host._host_port.begin();
	if (host.get_client_max_body_size() != -1)
		client_max_body_size = host.get_client_max_body_size();
	if (!host.get_error_pages().empty())
		_error_pages = host.get_error_pages();
	if (!host.get_root().empty())
		_root = host.get_root();	
	if (!_server_name.size())
		_server_name = *host.get_server_names().begin();

	std::map< std::string, location >::const_iterator it = get_most_accurate_location(host);
	if (it != host.get_locations().end()) {
		if (it->second.get_client_max_body_size() != -1)
			client_max_body_size = it->second.get_client_max_body_size();
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
		cgi_ext = it->second.get_cgi_ext();
		return ;
	}
}

std::string		config::update_path_to_target_with_root(const location & location) {
	if (_root.empty())
		return ("");

	std::string path_to_target; 
	int index_after_location = _request_target.find(_location, 0) + _location.size();
	std::string end_of_path = _request_target.substr(index_after_location, _request_target.size() - index_after_location);
	if (method & GET) {
		path_to_target = from_two_str_to_path(_root, end_of_path);
		if (is_directory(path_to_target))
			path_to_target = from_two_str_to_path(path_to_target, _index);
	}
	else if (method & POST) {
		path_to_target = from_two_str_to_path(_root, _upload_pass);
		path_to_target = from_two_str_to_path(path_to_target, end_of_path);
	}
	else if (method & DELETE)
		path_to_target = from_two_str_to_path(_root, end_of_path);
	return (path_to_target);
}

std::ostream & operator<<(std::ostream & o, const config & c)
{
	o << "HERE IS THE CONFIGURATION WE SET UP" << std::endl;
	o << "Server name: " << c._server_name << std::endl;
	o << "Location: " << c._location << std::endl;
	o << "Auto_index: " << c._auto_index << std::endl;
	o << "Client_max_body_size: " << c.client_max_body_size << std::endl;
	o << "Upload_pass: " << c._upload_pass << std::endl;
	o << "Root: " << c._root << std::endl;
	o << "CGI_dir: " << c._cgi_dir << std::endl;
	for (std::set<std::string>::const_iterator it = c.cgi_ext.begin(); it != c.cgi_ext.end(); it++)
		o << "CGI ext : " << *it << std::endl;
	o << "Index: " << c._index << std::endl;
	for (std::map< int, std::string >::const_iterator it = c._error_pages.begin(); it != c._error_pages.end(); it++)
		o << "Error " << it->first << " | page :" << it->second << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = c._header_fields.begin(); it != c._header_fields.end(); it++)
		o << "Header " << it->first << " | value :" << it->second << std::endl;
	o << "Return: " << c._return.first << " | Link :" << c._return.second << std::endl;
	o << "Method: " << ((c.method == GET) ? "GET" : (c.method == POST) ? "POST" : "DELETE") << std::endl;
	o << "Request_target: " << c._request_target << std::endl;
	o << "Final target: " << c.path_to_target << std::endl;
	o << "Query string: " << c.query_string << std::endl;
	o << "Path info: " << c.path_info << std::endl;
	
	return (o);
}

vHost & config::get_associated_vhost(request & request) {
	int first = 1;
	vHost *chosen;
	for (std::list<vHost>::iterator it = g_webserv._vhosts.begin(); it != g_webserv._vhosts.end(); it++) {
		if (it->get_csock_list().find(request.csock) != it->get_csock_list().end())
		{
			if (first) {
				chosen = &(*it);;
				first = 0;
			}
			if (it->get_server_names().find(request.host_name) 
					!= it->get_server_names().end()) {
				_server_name = request.host_name;
				chosen = &(*it);
				break ;
			}
		}
	}
	if (first == 1)
		g_logger.fd << g_logger.get_timestamp() << "--------We could not find any matching vhost, THAT SHOULD NOT HAPPEN---------" << std::endl;
	return (*chosen);
}

std::map< std::string, location >::const_iterator config::get_most_accurate_location(vHost & host) {
	std::string most_precise_location;
	std::map< std::string, location >::const_iterator it = host.get_locations().begin();
	for (; it != host.get_locations().end(); it++) {
		std::string begin_ofrequest_target = _request_target.substr(0, it->first.size());
		if (begin_ofrequest_target == it->first && begin_ofrequest_target.size() > most_precise_location.size()) {	
			most_precise_location = begin_ofrequest_target;
		}
	}
	return (host.get_locations().find(most_precise_location));		
}
