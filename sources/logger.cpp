/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 15:23:26 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/20 14:49:16 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.hpp"

logger::logger(std::string file) {
    this->fd.open(file.c_str(), std::ifstream::in | std::ifstream::trunc);
    if (!fd.is_open())
		std::cout << "not open\n";
}

std::ofstream & operator<<(logger & logger, const std::string & log) {
    std::time_t t = std::time(0);
    std::tm*    now = std::localtime(&t);

	logger.fd << now->tm_hour << "h" << now->tm_min << "m" << now->tm_sec << "s: " << log << std::endl;
    return (logger.fd);
}

logger::~logger() {
    this->fd.close();
}
