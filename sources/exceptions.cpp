/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:36:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 10:48:16 by ade-garr         ###   ########.fr       */
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

const char* empty_disabled_methods_declaration::what() const throw() {
	return ("You did not specify any method to disable at line: ");
}

const char* non_existing_disabled_methods::what() const throw() {
	return ("You specified a method that doesn't exists or that the server doesn't handle at line: ");
}

const char* empty_root_declaration::what() const throw() {
	return ("For the root directive, you did not specify any path at line: ");
}

const char* bad_auto_index_value::what() const throw() {
	return ("For the auto_index directive, you did not specify a good value (on or off) at line: ");
}

const char* empty_auto_index_declaration::what() const throw() {
	return ("For the auto_index directive, you did not specify a good value (on or off) at line: ");
}

const char* empty_error_page_declaration::what() const throw() {
	return ("For the error_page directive, you did not specify anything at line: ");
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

const char* empty_error_page_path::what() const throw() {
	return ("For the error_page directive, you did not specify any path at line: ");
}

bad_nb_argument::bad_nb_argument(std::string directive) : _directive(directive) {

	return ;
}

// const char* bad_nb_argument::what() const throw() {

// 	std::string ret;

// 	ret = "Bad number of argument for the directive: " + _directive;
// 	return (ret.c_str());
// }

