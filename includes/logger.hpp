/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/17 15:24:01 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:22:08 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "utils.hpp"

# include <fstream>
# include <ios>
# include <ctime>
# include <iostream>

# define LOG_START				"------BEGINNING OF THE PROGRAM-------"
# define LOG_CONFIG_DONE        "Config successful"
# define LOG_EPOLL_CREATE       "Epoll create successful			 | epfd  = "
# define LOG_LISTEN_SSOCK       "Ssock created           			 | ssock = "
# define LOG_SSOCK_ADD_EPOLL    "Ssock added to Epoll    			 | ssock = "
# define LOG_WAIT_CO    		"Epoll is waiting for connection"
# define LOG_EPOLL_EVENT   		"Epoll noticed new events     	| nb    = "
# define LOG_ISSUE_EPOLL_WAIT   "Epoll wait return -1     			 | nb    = "


class logger {

    public:
        logger(std::string file = "/logs/log");
        ~logger();


        logger & operator=(logger & rhs);
		friend std::ofstream    & operator<<(logger & logger, const std::string & log);
        std::string             	get_timestamp();

 
        std::ofstream    fd;
	private:
};



#endif