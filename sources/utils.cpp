/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 17:22:58 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/09 17:50:58 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

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
		throw bad_server_declaration();
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

std::pair<int, std::string> parse_return(std::string &line) {

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

std::string parse_upload_pass(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("upload_pass"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	return (str);
}

std::string parse_server_name(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("server_name"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	return (str);
}

void	parse_listen(std::string &line, vHost &host) {

    regex_t regex;
    int reti;
	int port;
	std::string addr;

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
        //host.setPort(std::atoi(str.c_str()));
		port = std::atoi(str.c_str());
    else {
    	regfree(&regex);
		throw (bad_port());
    }
	
    regfree(&regex);
	host.getHost_Port().push_back(std::pair<std::string, int>(addr, port));
	return ;
}

bool	is_ip(std::string str) {
	if (str.find('.', 0) == std::string::npos)
		return (0);
	return (1);
}

std::string	parse_ip(std::string str, vHost &host) {

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

void param_socket_server(vHost &host) { // a midif boucle sur les ports

    SOCKADDR_IN sin;
    SOCKET sock;
    socklen_t recsize = sizeof(sin);

	std::cout << "host = " << host.getHost() << std::endl;
	std::cout << "port = " << host.getPort() << std::endl;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		throw (cant_create_socket());
	sin.sin_addr.s_addr = inet_addr(host.getHost().c_str());
	if (sin.sin_addr.s_addr == INADDR_NONE && host.getHost() != "255.255.255.255")
		throw (bad_ip_address());
	sin.sin_family = AF_INET;
	sin.sin_port = htons(host.getPort());
	int sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
	if(sock_err == SOCKET_ERROR && errno != 48) {
		throw (cant_bind_address());
	}
	else if (sock_err == SOCKET_ERROR && errno == 48) {
		close(sock);
	}
	else {
		sock_err = listen(sock, 5); // 5 ? quel chiffre mettre ??
		if(sock_err == SOCKET_ERROR)
			throw (cant_listen());
		// static struct epoll_event ev;
		// ev.events = EPOLLIN | EPOLLET;
		// ev.data.fd = sock;
		// epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
	}


}
