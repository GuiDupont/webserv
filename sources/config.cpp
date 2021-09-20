/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/20 16:46:46 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/20 19:13:26 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

config::config(request & request) {
	_auto_index = g_webserv._auto_index;
	_client_max_body_size = g_webserv._client_max_body_size;
	_error_pages = g_webserv._error_pages;
	_upload_pass = g_webserv._upload_pass;
	_root = g_webserv._root;
	_cgi = g_webserv._cgi;

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
			if (it->get_server_names().find(request._header_fields.find("Host")->second) 
					!= it->get_server_names().end())
			{
				chosen = *it;
				break;
			}
		}
	}
	if (first)
		g_logger << "We could not find any matching vhost, THAT SHOULD NOT HAPPEN";
	put_vhost_and_location_in_config(chosen, request);
	
}

void	config::put_vhost_and_location_in_config(vHost & host, request & request) {
	_client_max_body_size = host.get_client_max_body_size();
	_error_pages = host.get_error_pages();
	_root = host.get_root();

	std::map< std::string, location >::const_iterator it;
	if ((it = host.get_locations().find(request._request_target)) != host.get_locations().end()) {
		_client_max_body_size = it->second.get_client_max_body_size();
		_auto_index = it->second.get_auto_index();
		_disable_methods = it->second.get_disable_methods(); 
		_error_pages = it->second.get_error_pages();
		_upload_pass = it->second.get_upload_pass();
		_root = it->second.get_root();
		_index = it->second.get_index();
		_return = it->second.get_return();
		_cgi_ext = it->second.get_cgi_ext();
		_path = it->second.get_path();
		return ;
	}
}
