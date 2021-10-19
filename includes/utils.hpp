/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 12:51:13 by ade-garr          #+#    #+#             */
/*   Updated: 2021/10/19 16:02:30 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "exceptions.hpp"
# include "webserv.hpp"
# include "vhost.hpp"
# include "request.hpp" 

# include <sys/stat.h>
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
# include <sys/stat.h>
# include <unistd.h>
# include <map>
# include <ftw.h>
# include <stdlib.h>
# include <stdint.h>

# define INVALID_SOCKET -1
# define SOCKET_ERROR -1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

class vHost;
class request;

int								go_to_next_word(const std::string & line, int index);
int								ft_string_is_alpha(const std::string & s);
int								ft_string_is_digit(const std::string & s);
bool                            ft_string_has_whitespace(const std::string & s);
int     						count_words(std::string &line);
std::string 					get_word(std::string line, size_t i);
std::string 					get_word(std::string & line, size_t &i, std::string delim);
bool							is_ip(std::string str);
void							param_socket_server(vHost &host);
std::string                     trims(std::string line, std::string chars);
bool	                        is_token(std::string & s);
bool	                        is_t_char(char c);
bool							is_field_content(std::string & s);
bool							is_field_vchar(unsigned char c);
std::string                     ft_itos(int n);
bool                            is_valid_request_target(std::string line);
int    							ft_atoi_base(const char *str, const char *base);
bool                            is_directory(const std::string & path);
void	                        stop_program_sigint(int signum);
std::string                     from_two_str_to_path(const std::string & str1, const std::string & str2);
bool    	        			test_path(request & request);
bool	                        is_valid_content_length(std::string val);
bool	                        is_chunked(request &req);
int                             find_word(std::string str, std::string word);
bool        					test_path_get(request & req);
bool        					test_path_delete(request & req);
bool        					test_path_post(request & req);
bool                            true_one_time_per_x_secondes(int x);
bool                            is_valid_file(std::string & path);
bool							is_symlink(std::string & path);
bool							is_file(std::string & path);
void                            add_fd_epollin_to_pool(int fd);
void                            add_fd_epollout_to_pool(int fd);
bool                            can_I_read_from_fd(int fd);
bool                            can_I_write_in_fd(int fd);
void                            remove_last_char_str(std::string & str);
bool                            are_two_path_equals(const std::string & path1, const std::string & path2);
bool                            is_EPOLLHUP(int fd);
void                            erase_static_fd_from_request(int fd);


#endif