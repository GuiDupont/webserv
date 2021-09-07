/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:36:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/07 17:27:42 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/exceptions.hpp"

const char* config_file_not_open::what() const throw() {
	return ("We can't open the config file at line ");
}

const char* bad_brackets_conf::what() const throw() {
	return ("There is an issue with the bracket disposition at line ");
}

const char* bad_directive::what() const throw() {
	return ("You asked for a directive that doesn't exist at line ");
}

const char* bad_client_max_body_size::what() const throw() {
	return ("You can not initialized such value for client_max_body_size at line ");
}

const char* bad_location_declaration::what() const throw() {
	return ("There is an issue at your location directive at line");
}

const char* no_port_associated::what() const throw() {
	return ("No port associated to the host server");
}

bad_nb_argument::bad_nb_argument(std::string &directive) : _directive(directive) {

	return ;
}

const char* bad_nb_argument::what() const throw() {

	std::string ret;

	ret = "Bad number of argument for the directive: " + _directive;
	return (ret.c_str());
}
