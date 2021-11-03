/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/20 16:46:46 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:54:02 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"


inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

void	change_upload_file_name(std::string & path_to_target) {
	
	size_t last_dot = path_to_target.rfind(".", path_to_target.size());
	if (last_dot == std::string::npos)
		path_to_target += ".1";
	else {
		std::string last_extension = path_to_target.substr(last_dot + 1, path_to_target.size());
		if (ft_string_is_digit(last_extension) == false)
			path_to_target += ".1";
		else {
			std::string new_path_to_target = path_to_target.substr(0, last_dot + 1);
			new_path_to_target += ft_itos(atoi(last_extension.c_str()) + 1);
			path_to_target = new_path_to_target;
		}
	}
}

config::config(void) :  return_activated(false), validity_checked(true), local_actions_done(false), cgi_activated(false) { }

config::config(request & request) : return_activated(false), validity_checked(false),  local_actions_done(false), cgi_activated(false) {
	if (request.method == "GET")
		method = GET;
	else if (request.method == "POST")
		method = POST;
	else if (request.method == "DELETE")
		method = DELETE;
	request.host_name = request.header_fields.find("host")->second;
	client_max_body_size = (g_webserv._client_max_body_size  == -1 ? 0: g_webserv._client_max_body_size);
	upload_pass = g_webserv._upload_pass;
	root = g_webserv._root;
	cgi_dir = g_webserv._cgi_dir;
	request_target = request.request_target;
	code = request.code_to_send;
	error_pages = g_webserv._error_pages;
	vHost chosen = get_associated_vhost(request);
	put_vhost_and_location_in_config(chosen);
	if (client_max_body_size <= 0 || client_max_body_size > MAX_BODY_SIZE)
		client_max_body_size = MAX_BODY_SIZE;
	set_cgi_params(request);
	for (std::map< int, std::string >::iterator it = g_webserv._error_pages.begin(); it != g_webserv._error_pages.end(); it++) {
		error_pages.insert(*it);
	}
	while (method == POST && cgi_activated == false && file_exists(path_to_target) && is_file(path_to_target))
		change_upload_file_name(path_to_target);
	// std::cout << *this << std::endl;
}


static size_t get_cgi_ext_pos(const std::string & target) {
	size_t start_search = 0;
	size_t start_cgi_ext = 0;

	while (1) {
		start_cgi_ext = target.find(".php", start_search);
		if (start_cgi_ext == std::string::npos)
			return (start_cgi_ext);
		else
		{
			start_search = start_cgi_ext + strlen(".php");
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
		return (target.size());
	return (index_query);
}

static void remove_upload_pass(std::string & target, const std::string & upload_pass, const std::string & root) {
	if (upload_pass.empty() || upload_pass == "\\" )
		return ;
	std::string first_part = target.substr(0, root.size());
	std::string second_part = target.substr(root.size() + upload_pass.size(), target.size() - (root.size() + upload_pass.size()));
	target = from_two_str_to_path(first_part, second_part);
}


void	config::set_cgi_params(request & req) {
	std::string &target = path_to_target;

	size_t begin_cgi_ext = 0;
	size_t first_diez = target.find_first_of("#", 0);
	if (first_diez != std::string::npos && first_diez)
		target = target.substr(0, first_diez - 1);
	begin_cgi_ext = get_cgi_ext_pos(target);
	size_t query_index = get_query_index(target, begin_cgi_ext);
	if (query_index != target.size())
		query_string = target.substr(query_index + 1, target.size() - (query_index + 1));
	target = target.substr(0, query_index);
	if (begin_cgi_ext == std::string::npos || cgi_ext.find(".php") == cgi_ext.end() || method & DELETE
		|| req.code_to_send != 0)
		return;
	size_t path_info_index = 0;
	if (begin_cgi_ext == std::string::npos)
		path_info_index = root.size();
	else
		path_info_index = begin_cgi_ext;
	path_info = target.substr(path_info_index, target.size() - path_info_index);
	if (path_info.empty() != true)
		path_info = from_two_str_to_path(root, path_info);
	target = target.substr(0, path_info_index);
	cgi_activated = true;
	size_t begin_of_script_name = root.size();
	script_name = target.substr(begin_of_script_name, target.size() - begin_of_script_name);
	script_name = from_two_str_to_path(location_name, script_name);
	remove_upload_pass(target, upload_pass, root);
}


void	config::put_vhost_and_location_in_config(vHost & host) {
	host_port = *host._host_port.begin();
	if (host.get_client_max_body_size() != -1)
		client_max_body_size = host.get_client_max_body_size();
	if (!host.get_root().empty())
		root = host.get_root();	
	if (!server_name.size() && host.get_server_names().size())
		server_name = *host.get_server_names().begin();
	std::map< std::string, class location >::const_iterator it = get_most_accurate_location(host);
	if (it != host.get_locations().end()) {
		if (it->second.get_client_max_body_size() != -1)
			client_max_body_size = it->second.get_client_max_body_size();
		auto_index = it->second.get_auto_index();
		disable_methods = it->second.get_disable_methods(); 
		if (!it->second.get_error_pages().empty())
			error_pages = it->second.get_error_pages();
		if (!it->second.get_upload_pass().empty())
			upload_pass = it->second.get_upload_pass();	
		location_name = it->second.get_location_name();
		if (!it->second.get_root().empty())
			root = it->second.get_root();
		index = it->second.get_index();
		path_to_target = update_path_to_target_with_root(it->second);
		return_pair = it->second.get_return();
		cgi_ext = it->second.get_cgi_ext();
		cgi_dir = it->second.get_cgi_dir();
	}
	if (!upload_pass.empty() && upload_pass[upload_pass.size() - 1] == '/')
			remove_last_char_str(upload_pass);
	for (std::map< int, std::string >::iterator it = host._error_pages.begin(); it != host._error_pages.end(); it++) {
		error_pages.insert(*it);
	}
}

std::string		config::update_path_to_target_with_root(const class location & location) {
	if (root.empty())
		return ("");

	std::string path_to_target; 
	int index_after_location = request_target.find(location_name, 0) + location_name.size();
	std::string end_of_path = request_target.substr(index_after_location, request_target.size() - index_after_location);
	if (method & GET) {
		path_to_target = from_two_str_to_path(root, end_of_path);
		if (is_directory(path_to_target) && are_two_path_equals(location.get_location_name(), request_target))
			path_to_target = from_two_str_to_path(path_to_target, index);
	}
	else if (method & POST) {
		path_to_target = from_two_str_to_path(root, upload_pass);
		path_to_target = from_two_str_to_path(path_to_target, end_of_path);
	}
	else if (method & DELETE)
		path_to_target = from_two_str_to_path(root, end_of_path);
	return (path_to_target);
}

std::ostream & operator<<(std::ostream & o, const config & c)
{
	o << "HERE IS THE CONFIGURATION WE SET UP" << std::endl;
	o << "Server name: " << c.server_name << std::endl;
	o << "Location: " << c.location_name << std::endl;
	o << "Auto_index: " << c.auto_index << std::endl;
	o << "Client_max_body_size: " << c.client_max_body_size << std::endl;
	o << "Upload_pass: " << c.upload_pass << std::endl;
	o << "Root: " << c.root << std::endl;
	o << "CGI_dir: " << c.cgi_dir << std::endl;
	for (std::set<std::string>::const_iterator it = c.cgi_ext.begin(); it != c.cgi_ext.end(); it++)
		o << "CGI ext : " << *it << std::endl;
	o << "Index: " << c.index << std::endl;
	for (std::map< int, std::string >::const_iterator it = c.error_pages.begin(); it != c.error_pages.end(); it++)
		o << "Error " << it->first << " | page :" << it->second << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = c.header_fields.begin(); it != c.header_fields.end(); it++)
		o << "Header " << it->first << " | value :" << it->second << std::endl;
	o << "Return: " << c.return_pair.first << " | Link :" << c.return_pair.second << std::endl;
	o << "Method: " << ((c.method == GET) ? "GET" : (c.method == POST) ? "POST" : "DELETE") << std::endl;
	o << "Request_target: " << c.request_target << std::endl;
	o << "Final target: " << c.path_to_target << std::endl;
	o << "Query string: " << c.query_string << std::endl;
	o << "Path info: " << c.path_info << std::endl;
	o << "Script name: " << c.script_name << std::endl;
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
			if (it->get_server_names().find(request.host_name)	!= it->get_server_names().end()) {
				server_name = request.host_name;
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
	std::map< std::string, class location >::const_iterator it = host.get_locations().begin();
	for (; it != host.get_locations().end(); it++) {
		std::string begin_ofrequest_target = request_target.substr(0, it->first.size());
		if (begin_ofrequest_target == it->first && begin_ofrequest_target.size() > most_precise_location.size()) {	
			most_precise_location = begin_ofrequest_target;
		}
	}
	return (host.get_locations().find(most_precise_location));		
}
