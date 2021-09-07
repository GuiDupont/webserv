

#include "../includes/utils.hpp"
<<<<<<< HEAD
#include <string>
#include "exceptions.hpp"
=======
#include "webserv.hpp"

#include <iostream>
>>>>>>> config

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

<<<<<<< HEAD
int	ft_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

void check_server_line(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("server"));
}

int count_words(std::string &line) {

	
=======
std::string get_word(std::string line, int i) {
	return (line.substr(i, line.find_first_of(WHITESPACE, i) - i));
>>>>>>> config
}