/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:42:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/17 18:36:04 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "webserv.hpp"
#include "utils.hpp"

webserv			g_webserv;
webserv_parser	g_parser;
logger			g_logger("logs/log"); // to fix

int main(int ac, char **av) {

	if (ac > 2) {
		std::cout << "Error argument\n" << std::endl;
		return (1);
	}
	try {
		g_logger << LOG_START;
		g_webserv = webserv(ac == 2 ? av[1] : "config/default.config");
	}
	catch (std::exception & e) {
		std::cout << e.what() << g_line << std::endl;
		return (1);
		//rajouter une ligne pour l'exception bad_nb_arg
	}
	try {
		g_webserv.set_hosts();
		g_webserv.wait_for_connection();
	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
		
	}
}
