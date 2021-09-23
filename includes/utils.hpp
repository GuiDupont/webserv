/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 12:51:13 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/23 12:09:41 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "exceptions.hpp"
# include "webserv.hpp"
# include "vhost.hpp"

# include <cctype>
# include <regex.h>
# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <unistd.h>
# include <cerrno>
# include <cstring>
# include <stdlib.h>

# define INVALID_SOCKET -1 // a bouger ?
# define SOCKET_ERROR -1 // a bouger ?

typedef int SOCKET; // a bouger ?
typedef struct sockaddr_in SOCKADDR_IN; // a bouger ?
typedef struct sockaddr SOCKADDR; // a bouger ?

class vHost;

int								go_to_next_word(const std::string & line, int index);
int								ft_string_is_alpha(const std::string & s);
int								ft_string_is_digit(const std::string & s);
int     						count_words(std::string &line);
std::string 					get_word(std::string line, int i);
std::string 					get_word(std::string & line, int &i, std::string delim);

bool							is_ip(std::string str);
void							param_socket_server(vHost &host);
std::string                     trims(std::string line, std::string chars);
bool	                        is_token(std::string & s);
bool	                        is_t_char(char c);
bool							is_field_content(std::string & s);
bool							is_field_vchar(unsigned char c);
std::string                     ft_itos(int n);
bool                            is_valid_request_target(std::string line);
void                            stop_program_sigint(int signum);
int    							ft_atoi_base(const char *str, const char *base);




#endif