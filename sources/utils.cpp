

#include "../includes/utils.hpp"

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
		if (!ft_isalpha(s[i]))
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
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}

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