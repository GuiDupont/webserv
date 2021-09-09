/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 17:22:58 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/09 12:12:24 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"
#include <string>
#include "exceptions.hpp"
#include "webserv.hpp"
#include <cctype>

#include <iostream>

int		go_to_next_word(const std::string & line, int index) {
	while (isspace(line[index])) index++;
	while (line[index] && !isspace(line[index])) index++;
	while (isspace(line[index])) index++;
	return (index);
}

int		ft_string_is_alpha(const std::string & s)
{
	int i = 0;
	while (s[i])
	{
		if (!isalpha(s[i]))
			return (0);
		i++;
	}
	return (1);
}

int		ft_string_is_digit(const std::string & s)
{
	int i = 0;
	while (s[i])
	{
		if (!isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

void check_server_line(std::string &line) {

	std::string	sd_word;
	int	i;

	if (count_words(line) != 2)
		throw (bad_nb_argument("server"));
	for (i = 0; i < line.size() && isspace(line[i]) == 1; i++) {}
	for (; i < line.size() && isspace(line[i]) == 0; i++) {}
	for (; i < line.size() && isspace(line[i]) == 1; i++) {}
	sd_word = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
	if (sd_word != "{")
		throw bad_nb_argument("server");
}

int count_words(std::string &line) {

	int count = 0;
	int i;

	for (i = 0; i < line.size(); i++) {
		if (isspace(line[i]) == 1) {}
		else {
			count++;
			for (; i < line.size() && isspace(line[i]) == 0; i++) {}
		}
	}
	return (count);
}

	
std::string get_word(std::string line, int i) {
	return (line.substr(i, line.find_first_of(WHITESPACE, i) - i));
}

std::pair<int, std::string> parse_error_page(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string code_str = get_word(line, i);
	if (!ft_string_is_digit(code_str))
		throw (bad_error_page_value());
	int code = atoi(code_str.c_str());
	if (code < 300 || code > 599 )
		throw (bad_error_page_value());
	i = go_to_next_word(line, i);
	std::string path = get_word(line, i);
	if (path.empty())
		throw (empty_declaration());
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_nb_argument("error_page"));
	return (std::pair<int, std::string>(code, path));	
	
}

size_t	get_max_body_size(std::string & line) {
	int i = go_to_next_word(line, 0);
	std::string max_size = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
	if (!ft_string_is_digit(max_size) || (line.find_first_of(WHITESPACE, i) != line.npos) )
		throw (bad_client_max_body_size());
	return (atoi(max_size.c_str()));
}
