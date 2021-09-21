/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/20 16:46:46 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/21 19:04:16 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

config::config(request & request) {
	_client_max_body_size = (g_webserv._client_max_body_size  == -1 ? 0: g_webserv._client_max_body_size);
	_error_pages = g_webserv._error_pages;
	_upload_pass = g_webserv._upload_pass;
	_root = g_webserv._root;
	_cgi_dir = g_webserv._cgi_dir;
	_request_target = request._request_target;
	_method = request._method;

	int first = 1;
	vHost chosen;
	for (std::list<vHost>::iterator it = g_webserv._vhosts.begin();
	 								it != g_webserv._vhosts.end(); it++) {
		if (it->get_csock_list().find(request._csock) != it->get_csock_list().end())
		{
			if (first) {
				chosen = *it;
				first = 0;
			}
			if (it->get_server_names().find(request._host) 
					!= it->get_server_names().end())
			{
				_server_name = request._host;
				chosen = *it;
				break;
			}
		}
	}
	if (first == 1)
		g_logger.fd << g_logger.get_timestamp() << "We could not find any matching vhost, THAT SHOULD NOT HAPPEN" << std::endl;
	put_vhost_and_location_in_config(chosen, request);
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

	std::map< std::string, location >::const_iterator it;
	if ((it = host.get_locations().find(request._request_target)) != host.get_locations().end()) {
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
		if (!it->second.get_root().empty())
			_root = it->second.get_root();
		_index = it->second.get_index();
		_return = it->second.get_return();
		_cgi_ext = it->second.get_cgi_ext();
		_location = it->second.get_location();
		return ;
	}
}

std::ostream & operator<<(std::ostream & o, const config & c)
{
	o << "HERE IS THE CONFIGURATION WE SET UP" << std::endl;
	o << "Server name: " << c._server_name << std::endl;
	o << "Request_target: " << c._location << std::endl;
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
	o << "Method: " << c._method << std::endl;
	o << "Request_target: " << c._request_target << std::endl;
	
	return (o);
}
