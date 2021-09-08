#include <string>


int			go_to_next_word(const std::string & line, int index);
int			ft_string_is_alpha(const std::string & s);
int			ft_string_is_digit(const std::string & s);
void    	check_server_line(std::string &line);
int     	count_words(std::string &line);
std::string get_word(std::string line, int i);

std::pair<int, std::string> parse_error_page(std::string & line);

size_t	get_max_body_size(std::string & line);
