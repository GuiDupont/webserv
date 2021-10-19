/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:36:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 16:04:19 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/exceptions.hpp"

const char* config_file_not_open::what() const throw() {
	return ("We can't open the config file at line: ");
}

const char* bad_brackets_conf::what() const throw() {
	return ("There is an issue with the bracket disposition at line: ");
}

const char* bad_directive::what() const throw() {
	return ("You asked for a directive that doesn't exist at line: ");
}

const char* bad_client_max_body_size::what() const throw() {
	return ("You can not initialized such value for client_max_body_size at line: ");
}

const char* bad_location_declaration::what() const throw() {
	return ("There is an issue at your location directive at line: ");
}

const char* no_port_associated::what() const throw() {
	return ("No port associated to the host server");
}

const char* non_existing_disabled_methods::what() const throw() {
	return ("You specified a method that doesn't exists or that the server doesn't handle at line: ");
}

const char* bad_auto_index_value::what() const throw() {
	return ("For the auto_index directive, you did not specify a good value (on or off) at line: ");
}

const char* empty_declaration::what() const throw() {
	return ("you did an empty declaration at line: ");
}

const char* bad_error_page_value::what() const throw() {
	return ("For the error_page directive, you did not specify a value between 300 and 599 at line: ");
}

const char* bad_nb_argument::what() const throw() {
	
	return ("Bad number of argument for the directive: ");
}

std::string bad_nb_argument::obj() {

	return (this->_directive);
}

bad_nb_argument::bad_nb_argument(std::string directive) : _directive(directive) {

	return ;
}

const char* bad_ip_address::what() const throw() {

	return ("Bad IP address given in config file");
}

const char* bad_ip_address_config::what() const throw() {

	return ("Bad IP address given at line: ");
}

const char* bad_port::what() const throw() {

	return ("Bad port given at line: ");
}

const char* bad_server_declaration::what() const throw() {
	
	return ("Bad declaration of a server at line: ");
}

const char* cant_create_socket::what() const throw() {

	return ("Impossible to create server socket");
}

const char* cant_bind_address::what() const throw() {
	return ("Impossible to bind address");
}

const char* cant_listen::what() const throw() {
	return ("Impossible to operate listen");
}

const char* epoll_ctl_add_error::what() const throw() {
	return ("Impossible to add sock to epoll");
}

const char* duplicate_location::what() const throw() {
	return ("Impossible to create two locations with the same path at line: ");
}

const char* port_already_used::what() const throw() {
	return ("Impossible to bind to port, it's might be used by another program.");
}

const char* config_is_dir::what() const throw() {
	return ("Config file should not be a directory. Ignore following int: ");
}