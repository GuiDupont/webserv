/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 17:22:58 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/08 23:26:35 by ade-garr         ###   ########.fr       */
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
	while (!isspace(line[index])) index++;
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

	if (count_words(line) != 2)
		throw (bad_nb_argument("server"));
	for (int i = 0; i < line.size() && isspace(line[i]) == 1; i++) {}
	for (; i < line.size() && isspace(line[i]) == 0; i++) {}
	for (; i < line.size() && isspace(line[i]) == 1; i++) {}
	sd_word = line.substr(i, line.find_first_of(" \t\n\v\f\r", i) - i);
	if (sd_word != "{")
		throw bad_nb_argument("server");
}

int count_words(std::string &line) {

	int count = 0;

	for (int i = 0; i < line.size(); i++) {
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