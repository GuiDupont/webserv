/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 17:22:58 by ade-garr          #+#    #+#             */
/*   Updated: 2021/10/19 16:02:19 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

int go_to_next_word(const std::string &line, int index)
{
	while (isspace(line[index]))
		index++;
	while (line[index] && !isspace(line[index]))
		index++;
	while (isspace(line[index]))
		index++;
	return (index);
}

int ft_string_is_alpha(const std::string &s)
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

int ft_string_is_digit(const std::string &s)
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

bool ft_string_has_whitespace(const std::string &s)
{
	int i = 0;
	while (s[i])
	{
		if (std::iswspace(s[i]))
			return (1);
		i++;
	}
	return (0);
}

int count_words(std::string &line)
{

	int count = 0;
	size_t i;

	for (i = 0; i < line.size(); i++) {
		if (isspace(line[i]) != 0)
			;
		else {
			count++;
			for (; i < line.size() && isspace(line[i]) == 0; i++)
				;
		}
	}
	return (count);
}

std::string get_word(std::string line, size_t i)
{
	return (line.substr(i, line.find_first_of(WHITESPACE, i) - i));
}

bool is_ip(std::string str)
{
	if (str.find('.', 0) == std::string::npos)
		return (0);
	return (1);
}

void param_socket_server(vHost &host)
{

	SOCKADDR_IN sin;
	SOCKET sock;
	socklen_t recsize = sizeof(sin);
	std::list<std::pair<std::string, size_t> >::iterator it = host.getHost_Port().begin();

	for (; it != host.getHost_Port().end(); it++)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
			throw(cant_create_socket());
		int optval = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&optval), sizeof(optval)))
			throw(cant_create_socket());
		sin.sin_addr.s_addr = inet_addr(it->first.c_str());
		if (sin.sin_addr.s_addr == INADDR_NONE && it->first != "255.255.255.255")
		{
			close(sock);
			throw(bad_ip_address());
		}
		sin.sin_family = AF_INET;
		sin.sin_port = htons(it->second);
		int sock_err = bind(sock, (SOCKADDR *)&sin, recsize);
		if (sock_err == SOCKET_ERROR && errno != EADDRINUSE)
		{
			close(sock);
			throw(cant_bind_address());
		}
		else if (sock_err == SOCKET_ERROR && errno == EADDRINUSE)
		{

			sock = g_webserv.get_sock_by_matching_host_ip(std::pair<std::string, size_t>(*it));
			if (sock == -1)
				throw(port_already_used());
			else
				host.map_sock_to_hostport(sock, *it);
		}
		else
		{
			sock_err = listen(sock, 100);
			if (sock_err == SOCKET_ERROR)
			{
				close(sock);
				throw(cant_listen());
			}
			g_logger.fd << g_logger.get_timestamp() << LOG_LISTEN_SSOCK << sock << std::endl;
			host.map_sock_to_hostport(sock, *it);
			static struct epoll_event ev;
			ev.events = EPOLLIN;
			ev.data.fd = sock;
			if (epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_ADD, sock, &ev) != 0)
				throw(epoll_ctl_add_error());
			g_logger.fd << g_logger.get_timestamp() << LOG_SSOCK_ADD_EPOLL << sock << std::endl;
		}
	}
}

std::string get_word(std::string &line, size_t &start_index, std::string delim)
{
	if (line.size() == 0 || !line[start_index])
		return ("");
	size_t delim_index = line.find(delim, start_index);
	if (delim_index == std::string::npos)
		return ("");
	std::string word = line.substr(start_index, delim_index - start_index);
	start_index = delim_index + delim.size();
	return (word);
}

std::string trims(std::string line, std::string chars)
{
	size_t index_begin = line.find_first_not_of(chars, 0);
	if (index_begin == std::string::npos)
		return ("");
	size_t index_end = line.find_last_not_of(chars, line.size());
	return (line.substr(index_begin, index_end + 1 - index_begin));
}

bool is_t_char(char c)
{
	if (std::isalnum(c) || c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || c == '\'' || c == '*' || c == '+' || c == '-' || c == '.' || c == '^' || c == '_' || c == '`' || c == '|' || c == '~')
		return (true);
	return (false);
}

bool is_token(std::string &s)
{
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

bool is_field_vchar(unsigned char c)
{
	if (std::isprint(c) || (c > 127 && c < 255))
		return (true);
	return (false);
}

bool is_field_content(std::string &s)
{
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
	int count = 1;

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
	long n_long;
	int count;
	char dest[20];

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

bool is_valid_request_target(std::string line)
{
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
		else {
			i = tmp;
			position++;
		}
		if (position < 0)
			return (false);
	}
	return (true);
}

void stop_program_sigint(int signum)
{
	(void)signum;
	std::set<int> csock;
	std::set<int> sock;
	g_logger.fd << g_logger.get_timestamp() << " We are about to quit" << std::endl;
	for (std::list<vHost>::iterator it = g_webserv.get_vhosts().begin(); it != g_webserv.get_vhosts().end(); it++) {
		for (std::set<int>::iterator it_csock = it->get_csock_list().begin(); it_csock != it->get_csock_list().end(); it_csock++)
			csock.insert(*it_csock);
		for (std::map<int, std::pair<std::string, size_t> >::iterator it_sock = it->get_sock_list().begin(); it_sock != it->get_sock_list().end(); it_sock++)
			sock.insert(it_sock->first);
	}
	for (std::set<int>::iterator it = csock.begin(); it != csock.end(); it++) {
		close(*it);
		// ? g_logger.fd << g_logger.get_timestamp() << " csock is closed: " << *it << std::endl : g_logger.fd << g_logger.get_timestamp() << " csock not closed: " << *it << " error: " << strerror(errno) << std::endl;
	}
	for (std::set<int>::iterator it = sock.begin(); it != sock.end(); it++) {
		close(*it); 
		//? g_logger.fd << g_logger.get_timestamp() << " cssock is closed: " << *it << std::endl : g_logger.fd << g_logger.get_timestamp() << " ssock not closed: " << *it << " error: " << strerror(errno) << std::endl;
	}

	close(g_webserv.get_epfd());
	// ? g_logger.fd << g_logger.get_timestamp() << " EPFD is closed: " << g_webserv.get_epfd() << std::endl : g_logger.fd << g_logger.get_timestamp() << " EPFD not closed: " << g_webserv.get_epfd() << " error: " << strerror(errno) << std::endl;
	g_logger.fd.close();
	free(g_webserv.get_revents());
	std::cout << std::endl << "Webserv is exiting" << std::endl;
	exit(1);
}

static int ft_retraitement(const char *str, int i, const char *base, int z)
{
	int j;
	int l;
	int m;

	l = 0;
	while (str[i] != '\0') {
		j = 0;
		m = 0;
		while (base[j] != '\0') {
			if (str[i] == base[j]) {
				l = l * z + j; // le retraitement a lieu ici
				m = 1;
			}
			j++;
		}
		if (m == 0)
			break;
		i++;
	}
	return (l);
}

static int ft_atoi(const char *str, const char *base, int z)
{
	int i;
	int k;
	int l;

	i = 0;
	k = 0;
	while (str[i] == ' ' || str[i] == '\f' || str[i] == '\n' ||
		   str[i] == '\r' || str[i] == '\t' || str[i] == '\v')
		i++;
	while (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			k++;
		i++;
	}
	l = ft_retraitement(str, i, base, z);
	if (k % 2 == 1)
		l = l * -1;
	return (l);
}

int ft_atoi_base(const char *str, const char *base)
{
	int i;
	int j;
	int k;

	i = 0;
	if (base[i] == '\0' || base[i + 1] == '\0')
		return (0);
	while (base[i] != '\0')
	{
		if (base[i] == '+' || base[i] == '-' || base[i] == ' ' ||
			base[i] == '\f' || base[i] == '\n' || base[i] == '\r' ||
			base[i] == '\t' || base[i] == '\v')
			return (0);
		j = i + 1;
		while (base[j] != '\0')
		{
			if (base[i] == base[j])
				return (0);
			j++;
		}
		i++;
	}
	k = ft_atoi(str, base, j);
	return (k);
}

bool is_directory(const std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (S_ISDIR(s.st_mode))
			return (true);
		return (false);
	}
	return (false);
}

bool is_file(std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (S_ISREG(s.st_mode))
			return (true);
		return (false);
	}
	// else {
	// 	g_logger.fd << g_logger.get_timestamp() << "Stat on " << path << " provides this error " << strerror(errno) << std::endl;
	// }
	return (false);
}

bool is_symlink(std::string &path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (S_ISLNK(s.st_mode))
			return (true);
		return (false);
	}
	// else {
	// 	g_logger.fd << g_logger.get_timestamp() << "Stat on " << path << " provides this error " << strerror(errno) << std::endl;
	// }
	return (false);
}

std::string from_two_str_to_path(const std::string &str1, const std::string &str2)
{
	std::string str2_bis = str2;
	if (str1[str1.size() - 1] == '/' && str2[0] == '/')
		str2_bis = str2.substr(1, str2.size() - 1);
	else if (str1[str1.size() - 1] != '/' && str2[0] != '/')
		str2_bis = "/" + str2_bis;
	return (str1 + str2_bis);
}

bool test_path_get(request &req)
{
	DIR *d;
	std::string path = req.conf->path_to_target;

	if (is_directory(path))
	{
		d = opendir(path.c_str());
		if (!d)
		{
			if (errno == EACCES)
				req.code_to_send = 403;
			else if (errno == ENOENT)
				req.code_to_send = 404;
			else
				req.code_to_send = 400;
			return (false);
		}
		free(d);
		if (req.conf->auto_index == false)
			req.body_response = " ";
		else
			req.body_response = response::generate_autoindex_body(req.conf->path_to_target, req.request_target);
		return (false);
	}
	int fd = open(path.c_str(), O_WRONLY);
	if (fd == -1)
	{
		g_logger.fd << g_logger.get_timestamp() << "can't open " << path << " - because : " << strerror(errno) << " " << errno << std::endl;
		if (errno == EACCES)
			req.code_to_send = 403;
		else if (errno == ENOENT || errno == ENOTDIR)
			req.code_to_send = 404;
		else
			req.code_to_send = 400;
	}
	close(fd);
	if (req.code_to_send != 0)
		return (false);
	return (true);
}

bool test_path_delete(request &req)
{
	DIR *d;
	std::string path = req.conf->path_to_target;

	if (is_directory(path))
	{
		d = opendir(path.c_str());
		if (!d)
		{
			if (errno == EACCES)
				req.code_to_send = 403;
			else if (errno == ENOENT)
				req.code_to_send = 404;
			else
				req.code_to_send = 400;
			return (false);
		}
		free(d);
		return (true);
	}
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		g_logger.fd << g_logger.get_timestamp() << "can't open " << path << " - because : " << strerror(errno) << std::endl;
		if (errno == EACCES)
			req.code_to_send = 403;
		else if (errno == ENOENT)
			req.code_to_send = 404;
		else
			req.code_to_send = 400;
	}
	close(fd);
	if (req.code_to_send != 0)
		return (false);
	return (true);
}

bool test_path_post(request &req)
{
	std::string path = req.conf->path_to_target;
	if (is_directory(path))
	{
		req.code_to_send = 400;
		// g_logger.fd << g_logger.get_timestamp() << "You can't POST a directory" << std::endl;
		return (false);
	}
	int fd = open(path.c_str(), O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	if (fd == -1)
	{
		g_logger.fd << g_logger.get_timestamp() << "can't open file cause : " << strerror(errno) << std::endl;
		if (errno == EACCES)
			req.code_to_send = 403;
		else if (errno == ENOENT)
			req.code_to_send = 404;
		else
			req.code_to_send = 400;
	}
	close(fd);
	if (req.code_to_send != 0)
		return (false);
	return (true);
}

bool is_valid_content_length(std::string val)
{

	regex_t regex;
	int reti;

	reti = regcomp(&regex, "^[0-9]\\{0,10\\}$", 0);
	reti = regexec(&regex, val.c_str(), 0, NULL, 0);
	if (reti)
	{
		regfree(&regex);
		return (0);
	}
	regfree(&regex);
	return (1);
}

bool is_chunked(request &req)
{

	std::map<std::string, std::string>::iterator it = req.header_fields.find("transfer-encoding");
	if (it == req.header_fields.end())
		return (0);
	int index = find_word(it->second, "chunked");
	if (index >= 0)
		return (1);
	else
		return (0);
}

int find_word(std::string str, std::string word)
{

	std::string::iterator it = str.begin();
	std::string::iterator it2;
	std::string::iterator it3;
	std::string substr;

	if (str.size() == 0 || word.size() == 0)
		return (-1);
	for (; it != str.end(); it++)
	{
		for (; it != str.end() && std::isspace(*it) != 0; it++)
		{
		}
		if (str.find(',', it - str.begin()) != std::string::npos)
			it2 = str.begin() + str.find(',', it - str.begin());
		else
			it2 = str.end();
		substr = str.substr(it - str.begin(), it2 - it);
		it3 = it2 - 1;
		for (; it3 - it >= 0 && std::isspace(*it3) != 0; it3--)
		{
		}
		substr = substr.substr(0, it3 - it + 1);
		if (substr == word)
			return (it - str.begin());
		it = (it2 == str.end() ? it2 - 1 : it2);
	}
	return (-1);
}

bool true_one_time_per_x_secondes(int x)
{
	static time_t last_call = time(0);
	if (time(0) - last_call >= x)
	{
		last_call = time(0);
		return (true);
	}
	return (false);
}

bool is_valid_file(std::string &path)
{
	if (is_directory(path))
		return (false);
	int fd = open(path.c_str(), O_WRONLY);
	if (fd == -1)
		return (false);
	close(fd);
	return (true);
}

void add_fd_epollin_to_pool(int fd)
{
	static struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_ADD, fd, &ev) == -1)
		g_logger.fd << g_logger.get_timestamp() << "We had an issue with EPOLL CTL, trying to add " << fd << " to the pool. errno :" << errno << ": " << strerror(errno) << std::endl;
	// else
	// 	g_logger.fd << g_logger.get_timestamp() << "We added " << fd << " to the pool looking, for Epollin." << std::endl;
}

void add_fd_epollout_to_pool(int fd)
{
	static struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = fd;
	if (epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_ADD, fd, &ev) == -1)
		g_logger.fd << g_logger.get_timestamp() << "We had an issue with EPOLL CTL EPOLLOUT, trying to add " << fd << " to the pool. errno :" << errno << ": " << strerror(errno) << std::endl; // add an exception
}

bool can_I_write_in_fd(int fd)
{
	struct epoll_event *_revents = g_webserv.get_revents();
	int nsfd = g_webserv.nsfd;

	for (int i = 0; i < nsfd; i++)
	{
		if (_revents[i].data.fd == fd && _revents[i].events & EPOLLOUT)
			return true;
	}

	return false;
}

bool can_I_read_from_fd(int fd)
{
	struct epoll_event *_revents = g_webserv.get_revents();
	int nsfd = g_webserv.nsfd;

	for (int i = 0; i < nsfd; i++)
	{
		if (_revents[i].data.fd == fd && _revents[i].events & EPOLLIN)
		{
			return true;
		}
	}
	return false;
}

void remove_last_char_str(std::string &str)
{
	if (str.empty())
		return;
	str = str.substr(0, str.size() - 1);
}

bool are_two_path_equals(const std::string &path1, const std::string &path2)
{
	if (path1 == path2)
		return (true);

	std::string copy1 = path1;
	std::string copy2 = path2;
	
	if (copy1[copy1.size() - 1] == '/')
		copy1 = copy1.substr(0, copy1.size() - 1);
	if (copy1[0] == '/')
		copy1 = copy1.substr(1, copy1.size() - 1);

	if (copy2[copy2.size() - 1] == '/')
		copy2 = copy2.substr(0, copy2.size() - 1);
	if (copy2[0] == '/')
		copy2 = copy2.substr(1, copy2.size() - 1);
	if (copy1 == copy2)
		return (true);
	return (false);
}

bool is_EPOLLHUP(int fd) {
	struct epoll_event* _revents = g_webserv.get_revents();
	int nsfd					= g_webserv.nsfd;

	for (int i = 0; i < nsfd; i++) {
		if (_revents[i].data.fd == fd && _revents[i].events & EPOLLHUP) {
			g_logger.fd << g_logger.get_timestamp() << "fd " << fd << " has EPOLLHUP" << std::endl;
			return true;
		}
	}
	return false;
}

void	erase_static_fd_from_request(int fd) {
	g_webserv.static_fds.erase(fd);
	int ret = epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_DEL, fd, NULL);
	if (ret == -1)
		g_logger.fd << g_logger.get_timestamp() << "epoll_ctl failed " << errno << std::endl;
}
