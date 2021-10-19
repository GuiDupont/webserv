/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:42:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:54:51 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "webserv.hpp"
#include "utils.hpp"

webserv			g_webserv;
webserv_parser	g_parser;
logger			g_logger("logs/log"); // to fix



int main(int ac, char **av) {
	signal(SIGINT, stop_program_sigint);
	signal(SIGPIPE, SIG_IGN);

	if (ac > 2) {
		std::cout << "Error argument\n" << std::endl;
		return (1);
	}
	try {
		g_logger.fd << g_logger.get_timestamp() << LOG_START << std::endl;
		g_webserv = webserv(ac == 2 ? av[1] : "config/default.config");
	}
	catch (std::exception & e) {
		std::cout << e.what() << g_line << std::endl;
		return (1);
	}
	try {
		g_webserv.set_hosts();
		std::cout << "Webserv is running" << std::endl;
		g_webserv.wait_for_connection();
	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	stop_program_sigint(1);
}
