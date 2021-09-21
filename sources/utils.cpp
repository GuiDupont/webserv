/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 17:22:58 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/21 15:42:05 by gdupont          ###   ########.fr       */
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

int count_words(std::string &line) {

	int count = 0;
	int i;

	for (i = 0; i < line.size(); i++) {
		if (isspace(line[i]) != 0) {}
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

bool	is_ip(std::string str) {
	if (str.find('.', 0) == std::string::npos)
		return (0);
	return (1);
}

void param_socket_server(vHost &host) {

    SOCKADDR_IN sin;
    SOCKET sock;
    socklen_t recsize = sizeof(sin);
	std::list< std::pair< std::string, size_t> >::iterator it = host.getHost_Port().begin();
	
	for (; it != host.getHost_Port().end(); it++) {
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
			throw (cant_create_socket());
		sin.sin_addr.s_addr = inet_addr(it->first.c_str());
		if (sin.sin_addr.s_addr == INADDR_NONE && it->first != "255.255.255.255") {
			close(sock);
			throw (bad_ip_address());
		}
		sin.sin_family = AF_INET;
		sin.sin_port = htons(it->second);
		int sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
		if(sock_err == SOCKET_ERROR && errno != EADDRINUSE) {
			close(sock);
			throw (cant_bind_address());
		}
		else if (sock_err == SOCKET_ERROR && errno == EADDRINUSE) {
			close(sock);
			sock = g_webserv.get_sock_by_matching_host_ip(std::pair< std::string, size_t> (*it));
			if (sock == -1)
				throw (port_already_used());
			else
				host.map_sock_to_hostport(sock, *it);
		}
		else {
			sock_err = listen(sock, 5); // 5 ? quel chiffre mettre ??
			if (sock_err == SOCKET_ERROR) {
				close(sock);
				throw (cant_listen());
			}
			g_logger << LOG_LISTEN_SSOCK + ft_itos(sock);
			host.map_sock_to_hostport(sock, *it); // tester avec config mm port/dif address..
			static struct epoll_event ev;
			ev.events = EPOLLIN | EPOLLOUT | EPOLLET; // a voir si EPOLLET necessaire
			ev.data.fd = sock;
			if (epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_ADD, sock, &ev) != 0)
				throw (epoll_ctl_add_error());
			g_logger << LOG_SSOCK_ADD_EPOLL + ft_itos(sock);
		}
	}
}

std::string get_word(std::string & line, int &start_index, std::string delim) {
	if (line.size() == 0 || !line[start_index])
		return ("");
	int delim_index = line.find(delim, start_index);
	if (delim_index == std::string::npos)
		return ("");
	std::string word = line.substr(start_index, delim_index - start_index);
	start_index = delim_index + delim.size();
	return (word);
}

std::string trims(std::string  line, std::string chars) {
	size_t index_begin = line.find_first_not_of(chars, 0);
	if (index_begin == std::string::npos)
		return ("");
	size_t index_end = line.find_last_not_of(chars, line.size());
	return (line.substr(index_begin, index_end + 1 - index_begin));	
}

bool	is_t_char(char c) {
	if (std::isalnum(c) || c == '!' || c ==  '#' || c ==  '$' || c ==  '%' 
	|| c ==  '&' || c ==  '\'' || c ==  '*' || c ==  '+' || c ==  '-' || c ==  '.' 
	|| c ==  '^' || c ==  '_' || c ==  '`' || c ==  '|' || c ==  '~')
		return (true);
	return (false);
}

bool	is_token(std::string & s) {
	int index = 0;

	if (!s.size())
		return (false);
	while (s[index])
	{
		if (!is_t_char(s[index++]))
			return (false);
	}
	return (true);
}

bool	is_field_vchar(unsigned char c) {
	if (std::isprint(c) || (c > 127 && c < 255))
		return (true);
	return (false);
}

bool	is_field_content(std::string & s) {
	int index = 0;
	
	if (!is_field_vchar(s[0]))
		return (false);
	if (!s.size())
		return (false);
	while (s[index])
	{
		if (!is_field_vchar(s[index]) && s[index] != '\t')
			return (false);
		index++;	
	}
	return (true);
}


static int size_dest(long n)
{
	char	*dest;
	int		count = 1;

	if (n < 0)
	{
		n = -n;
		count++;
	}
	while (n >= 10)
	{
		n = n / 10;
		count++;
	}
	return (count);
}

std::string ft_itos(int n)
{
	long	n_long;
	int		count;
	char	dest[20];

	n_long = n;
	count = size_dest(n);
	dest[count] = '\0';
	if (n_long < 0)
	{
		dest[0] = '-';
		n_long = -n_long;
	}
	count--;
	while (n_long > 9)
	{
		dest[count] = n_long % 10 + '0';
		n_long = n_long / 10;
		count--;
	}
	dest[count] = n_long % 10 + '0';
	return (std::string(dest));
}
 
bool	is_valid_request_target(std::string line) {
	int position = 0;
	size_t tmp;
	
	for (int i = 0; line[i];) {
		if (line[i] == '/')
			i++;
		else if (line.substr(i, 3) == "../") {
			position--;
			i += 3;
		}
		else if (line.substr(i, 2) == ".." && !line[i + 2]) {
			position--;
			i += 2;
		}
		else if ((tmp = line.find("/", i)) == line.npos)
			i = line.size();
		else if (line.substr(i, 2) == "./")
			i = tmp;
		else
		{
			i = tmp;
			position++;
		}
		if (position < 0)
			return (false);
	}
	return (true);
}

void	stop_program_sigint(int signum) {
	std::set<int> csock;
	std::set<int> sock;
	for (std::list<vHost>::iterator it = g_webserv.get_vhosts().begin(); it != g_webserv.get_vhosts().end(); it++) {
		for (std::set<int>::iterator it_csock = it->get_csock_list().begin(); it_csock != it->get_csock_list().end(); it_csock++)
			csock.insert(*it_csock);
		for (std::map< int, std::pair< std::string, size_t> >::iterator it_sock = it->get_sock_list().begin(); it_sock != it->get_sock_list().end(); it_sock++) {
			sock.insert(it_sock->first);
		}
	}
	for (std::set<int>::iterator it = csock.begin(); it != csock.end(); it++) {
		g_logger.fd << g_logger.get_timestamp() << " csock is closed: " << *it << std::endl;
		close(*it);
	}
	for (std::set<int>::iterator it = sock.begin(); it != sock.end(); it++) {
		g_logger.fd << g_logger.get_timestamp() << " ssock is closed: " << *it << std::endl;
		close(*it);
	}
	g_logger.fd << g_logger.get_timestamp() << " EPFD is closed: " << g_webserv.get_epfd() << std::endl;
	close(g_webserv.get_epfd());
	exit(1);
}
// to test function

// std::string test = "/..";
// 	std::cout << test << " : " << (is_valid_request_target(test) ? "valid\n" : "not valid\n");
// 	test = "/sdfas/dsfdsf/../sdfs/..";
// 	std::cout << test << " : " << (is_valid_request_target(test) ? "valid\n" : "not valid\n");
// 	test = "/sfsf/../../";
// 	std::cout << test << " : " << (is_valid_request_target(test) ? "valid\n" : "not valid\n");
// 	test = "/sfsd/.././dfsfs/./sfsdf/.sfsfs/..fsdfs/../../";
// 	std::cout << test << " : " << (is_valid_request_target(test) ? "valid\n" : "not valid\n");
// 	test = "/lolo/dsfsd/sfsd/./../../../..";
// 	std::cout << test << " : " << (is_valid_request_target(test) ? "valid\n" : "not valid\n");