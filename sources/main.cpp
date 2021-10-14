/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:42:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/14 17:52:29 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "webserv.hpp"
#include "utils.hpp"

webserv			g_webserv;
webserv_parser	g_parser;
logger			g_logger("logs/log"); // to fix


static void remove_upload_pass(std::string & target, const std::string & upload_pass, const std::string & root) {
	if (upload_pass.empty() || upload_pass == "/" )
		return ;
	std::string first_part = target.substr(0, root.size());
	std::string second_part = target.substr(root.size() + upload_pass.size(), target.size() - (root.size() + upload_pass.size()));
	target = from_two_str_to_path(first_part, second_part);
}



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
		//rajouter une ligne pour l'exception bad_nb_arg
	}
	try {
		g_webserv.set_hosts();
		g_webserv.wait_for_connection();
	}
	catch (std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	stop_program_sigint(1);
	// quitter proprement
}
