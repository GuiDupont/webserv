/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 12:51:13 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/09 12:52:35 by ade-garr         ###   ########.fr       */
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
void	parse_ip(std::string str, vHost &host);

#endif