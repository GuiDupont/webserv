/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 12:51:13 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/10 11:19:10 by ade-garr         ###   ########.fr       */
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


# define INVALID_SOCKET -1 // a bouger ?
# define SOCKET_ERROR -1 // a bouger ?

typedef int SOCKET; // a bouger ?
typedef struct sockaddr_in SOCKADDR_IN; // a bouger ?
typedef struct sockaddr SOCKADDR; // a bouger ?

class vHost;

int			go_to_next_word(const std::string & line, int index);
int			ft_string_is_alpha(const std::string & s);
int			ft_string_is_digit(const std::string & s);
void    	check_server_line(std::string &line);
int     	count_words(std::string &line);
std::string get_word(std::string line, int i);
std::pair<int, std::string> parse_error_page(std::string & line);
size_t	get_max_body_size(std::string & line);
std::pair<int, std::string> parse_return(std::string &line);
std::string parse_upload_pass(std::string &line);
std::string parse_server_name(std::string &line);
void	parse_listen(std::string &line, vHost &host);
bool	is_ip(std::string str);
std::string	parse_ip(std::string str, vHost &host);
void	param_socket_server(vHost &host);

#endif