/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 17:22:58 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/16 10:13:14 by gdupont          ###   ########.fr       */
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
				;// throw
			else
				host.map_sock_to_hostport(sock, *it);
		}
		else {
			sock_err = listen(sock, 5); // 5 ? quel chiffre mettre ??
			if (sock_err == SOCKET_ERROR) {
				close(sock);
				throw (cant_listen());
			}
			host.map_sock_to_hostport(sock, *it); // tester avec config mm port/dif address..
			static struct epoll_event ev;
			ev.events = EPOLLIN | EPOLLET | EPOLLOUT; // a voir si EPOLLET necessaire
			ev.data.fd = sock;
			if (epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_ADD, sock, &ev) != 0)
				throw (epoll_ctl_add_error());
		}
	}
	// std::map< int, std::pair< std::string, size_t> >::iterator it2 = host.get_sock_list().begin();
	// for (; it2 != host.get_sock_list().end(); it2++)
	// {
	// 	std::cout << "Sock : " << it2->first << " : Host : " << it2->second.first << " : PORT : " << it2->second.second << std::endl;
	// }
}

std::string get_word(std::string & line, int &start_index, std::string & delim) {
	if (line.size() == 0)
		return ("");

	int delim_index = line.find(delim, start_index);
	std::string word = line.substr(start_index, delim_index - start_index);
	start_index = delim_index;
	return (word);
}
