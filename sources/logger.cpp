/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 15:23:26 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/17 17:27:57 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "logger.hpp"

logger::logger(std::string file) {
    std::cout << file.c_str();
    this->fd.open(file.c_str(), std::ifstream::in);
    if (!fd.is_open())
		std::cout << "not open\n";
}

std::ofstream & operator<<(logger & logger, const std::string & log) {
    std::time_t t = std::time(0);
    std::tm*        now = std::localtime(&t);

	logger.fd << now->tm_hour << "h" << now->tm_min << "m" << now->tm_sec << "s: " << log << std::endl;
    return (logger.fd);
}

logger::~logger() {
    this->fd.close();
}

// logger & logger::operator=(logger & rhs) {
//     f = rhs.f;
// }
