/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 14:03:24 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/20 14:23:28 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

/* BRACKETS */

bool		webserv_parser::check_brackets(const std::string & config) {
	size_t 	position = 0;
	while (1) {
		position = config.find('{', position);
		if (position == std::string::npos)
			break ;
		position = get_next_closing_bracket(config, position + 1);
		if (position == std::string::npos)
			return (false);
	}
	return (check_closing_brackets(config));
}


size_t		webserv_parser::get_next_closing_bracket(const std::string & config, size_t position) {
	size_t next_open_bracket = config.find('{', position);
	size_t next_closing_bracket = config.find('}', position);
	
	if (next_closing_bracket == std::string::npos)
		return (std::string::npos);
	if (next_open_bracket < next_closing_bracket)
	{
		position = get_next_closing_bracket(config, next_open_bracket + 1);
		if (position == std::string::npos)
			return (std::string::npos);
		position = config.find('}', position + 1);
		if (position == std::string::npos)
			return (std::string::npos);
		else
			return (position);
	}
	return (next_closing_bracket);
}

bool 		webserv_parser::check_closing_brackets(const std::string & config) {
	
	int open_count = 0;
	int close_count = 0;
	int position = 0;

	while (config.find('{', position) != std::string::npos) {
		position = config.find('{', position) + 1;
		open_count++;
	}
	position = 0;
	while (config.find('}', position) != std::string::npos) {
		position = config.find('}', position) + 1;
		close_count++;
	}
	if (close_count != open_count)
		return (false);
	else
		return (true);	
}

/* END BRACKETS */

/* 		UTILS		*/

size_t		webserv_parser::parse_disabled_methods(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string method;
	size_t disabled_methods = 0;
	while (1) {
		method = get_word(line, i);
		if (method == "GET")
			disabled_methods |= GET;
		else if (method == "POST")
			disabled_methods |= POST;
		else if (method == "DELETE")
			disabled_methods |= DELETE;
		else
			throw (non_existing_disabled_methods());
		i = go_to_next_word(line, i);
		if (!line[i])
			return (disabled_methods);
	}
	return (disabled_methods);
}

std::string	webserv_parser::parse_one_word(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());// change with empty 
	std::string path = get_word(line, i);
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_directive()); // change with wrong nb_arg
	return (path);
}


bool	webserv_parser::parse_auto_index(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string on_off = get_word(line, i);
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_directive()); //change with wrong nb argument
	if (on_off == "on")
		return true;
	if (on_off == "off")
		return false;
	else
		throw (bad_auto_index_value()); //change with wrong nb argument
}

void	webserv_parser::parse_cgi_extension(std::list< std::string > & cgi_ext, std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	while (line[i]) {
		cgi_ext.push_back(get_word(line, i));
		i = go_to_next_word(line, i);
	}
}

size_t	webserv_parser::get_max_body_size(std::string & line) {
	int i = go_to_next_word(line, 0);
	std::string max_size = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
	if (!ft_string_is_digit(max_size) || (line.find_first_of(WHITESPACE, i) != line.npos) )
		throw (bad_client_max_body_size());
	return (atoi(max_size.c_str()));
}


std::pair<int, std::string> webserv_parser::parse_return(std::string &line) {

	if (count_words(line) != 3)
		throw (bad_nb_argument("return"));
	int i = go_to_next_word(line, 0);
	std::string code_str = get_word(line, i);
	if (!ft_string_is_digit(code_str))
		throw (bad_error_page_value());
	int code = atoi(code_str.c_str());
	if (code < 0 || code > 999 )
		throw (bad_error_page_value());
	i = go_to_next_word(line, i);
	std::string path = get_word(line, i);
	return (std::pair<int, std::string>(code, path));
}

std::pair<int, std::string> webserv_parser::parse_error_page(std::string & line) {
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

std::string webserv_parser::parse_server_name(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("server_name"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	return (str);
}

void	webserv_parser::parse_listen(std::string &line, vHost &host) {

    regex_t regex;
    int reti;
	int port;
	std::string addr("0.0.0.0");

	if (count_words(line) != 2)
		throw (bad_nb_argument("listen"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	if (is_ip(str) == 1) {
		addr = parse_ip(str, host);
		str = str.substr(str.find(':', 0) + 1, (str.size() - str.find(':', 0) + 1));
	}
	reti = regcomp(&regex, "^[0-9]\\{0,5\\}$", 0);
    reti = regexec(&regex, str.c_str(), 0, NULL, 0);
	if( !reti )
		port = std::atoi(str.c_str());
    else {
    	regfree(&regex);
		throw (bad_port());
    }
    regfree(&regex);
	host.getHost_Port().push_back(std::pair<std::string, int>(addr, port));
	return ;
}

std::string webserv_parser::parse_upload_pass(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("upload_pass"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	return (str);
}

std::string	webserv_parser::parse_ip(std::string str, vHost &host) {

        regex_t regex;
        int reti;
		std::string addr;

        reti = regcomp(&regex, "^[0-9]\\{1,3\\}[.][0-9]\\{1,3\\}[.][0-9]\\{1,3\\}[.][0-9]\\{1,3\\}:", 0);
        reti = regexec(&regex, str.c_str(), 0, NULL, 0);
        if( !reti )
            addr = str.substr(0, str.find(':', 0));
        else {
			regfree(&regex);
			throw (bad_ip_address());
        }
    	regfree(&regex);
        return (addr);
}

void 		webserv_parser::check_server_line(std::string &line) {

	std::string	sd_word;
	int	i;

	if (count_words(line) != 2)
		throw (bad_nb_argument("server"));
	for (i = 0; i < line.size() && std::isspace(line[i]) != 0; i++) {}
	for (; i < line.size() && std::isspace(line[i]) == 0; i++) {}
	for (; i < line.size() && std::isspace(line[i]) != 0; i++) {}
	sd_word = line.substr(i, line.size() - i);
	if (sd_word != "{")
		throw bad_server_declaration();
}

std::pair<std::string, std::string> webserv_parser::get_header_begin_body(int csock) { // a supprimer ?? (ancienne fonction pour avoir le bdy)
	std::string big_buffer;

	char c_buffer[1025];
	int ret;
	size_t end_header_index = 0;
	int last_index = 3;

	while (1)
	{
		ret = recv(csock, c_buffer, 1024, 0);
		if (ret < 0)
		{
			std::cout << strerror(errno) << std::endl;
			return (std::pair<std::string, std::string>("", ""));
		}
		c_buffer[ret] = '\0';
		big_buffer += c_buffer;
		if ((end_header_index = big_buffer.find("\r\n\r\n", last_index - 3)) != std::string::npos)
		{
			return (std::pair<std::string, std::string>(big_buffer.substr(0, end_header_index + 4), 
			big_buffer.substr(end_header_index + 4, big_buffer.size() - (end_header_index + 4))));
		}
		last_index = (big_buffer.size() - 1 > 3) ? big_buffer.size() - 1 : 3;
	}
	return (std::pair<std::string, std::string>("", ""));
}
