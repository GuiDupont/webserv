/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 15:23:26 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:38:22 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.hpp"

logger::logger(std::string file) {
    this->fd.open(file.c_str(), std::ifstream::in | std::ifstream::trunc);
    if (!fd.is_open())
		std::cout << "Log file not open : " << strerror(errno) << std::endl;
}

logger::~logger() {
    this->fd.close();
}

std::string     logger::get_timestamp() {

	signal(SIGINT, SIG_IGN);
    std::time_t t = std::time(0);
    std::tm*  now = std::localtime(&t);
	signal(SIGINT, stop_program_sigint);
    std::string timestamp;
	timestamp =  ft_itos(now->tm_hour) + "h" + ft_itos(now->tm_min) + "m" + ft_itos(now->tm_sec) + "s: ";

    return (timestamp);
}
