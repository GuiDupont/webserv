

#include "../includes/utils.hpp"
#include <string>
#include "exceptions.hpp"
#include "webserv.hpp"

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

	if (count_words(line) != 2)
		throw (bad_nb_argument("server"));
}

int count_words(std::string &line) {

	
std::string get_word(std::string line, int i) {
	return (line.substr(i, line.find_first_of(WHITESPACE, i) - i));
}