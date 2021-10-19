/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:58:31 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:34:41 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "parser.hpp"
# include "vhost.hpp"
# include "utils.hpp"
# include "request.hpp"
# include "exceptions.hpp"
# include "logger.hpp"
# include "response.hpp"

# include <list>
# include <map>
# include <string>
# include <cstdlib>
# include <cctype>
# include <sys/epoll.h>
# include <fstream>
# include <iostream>
# include <fcntl.h>
# include <ctime>
# include <csignal>
# include <regex.h>
# include <sys/socket.h>
# include <stdio.h>

# define WHITESPACE " \t\n\v\f\r"

# define GET 1
# define POST 2
# define DELETE 4

# define TIMEOUT 50

# define SEND_SPEED 10000

# define REVENT_SIZE 64

class vHost;
class request;
class response;

extern class webserv_parser g_parser;
extern class logger g_logger;

typedef struct sockaddr SOCKADDR;


class webserv {
	
	friend class webserv_parser;
	friend class config;

	public:
		std::map<int, std::string>					status_code;
		std::map<int, request>						requests;
		std::set<int>								static_fds;
		std::set<int>								static_fds_to_close;
		int											nsfd;

	private:
		std::list<vHost> 							_vhosts;
		int											_client_max_body_size;
		std::map< int, std::string >				_error_pages;
		std::string									_upload_pass;
		std::string 								_root;
		std::string									_cgi_dir;
		int											_epfd;
		std::map<int, std::time_t>					_timeout;
		struct epoll_event							*_revents;
		
	public:
		webserv(const std::string & path_config);
		webserv();
		~webserv();

		void	set_config(std::ifstream & config_file);

		void 								set_hosts();
		int									get_epfd() const ;
		std::list<vHost>					&get_vhosts() ;
		std::string							&get_root();
		std::map< int, std::string > const	&get_error_pages() const;

		void	wait_for_connection();
		void	handle_new_client(int ssock, SOCKADDR* csin, socklen_t* crecsize);
		void	answer_to_request(int csock);


		bool	ft_is_ssock(int fd);
		bool	ft_is_static_fd(int fd);
		bool	ft_is_static_fd_to_close(int fd);
		void	ft_add_csock_to_vhost(int sock, int csock);
		int		get_sock_by_matching_host_ip(std::pair< std::string, size_t> host_port);
		void	display_sock();
		bool	is_pending_request(int csock);
		bool	is_new_request(int fd);
		void	control_time_out(void);
		void	read_from_csock(int csock);
		void	analyse_header(request &req);
		void	analyse_body(request &req);
		void	set_request_to_ended(request &req);
		void	clean_csock_from_server(int fd);
		int		find_word(std::string str, std::string word);
		void	insert_status_code();
		void	accept_new_client(int sock);
		void	handle_pollin_csock(int csock);
		struct epoll_event							*get_revents();
	private:


};
extern webserv g_webserv;



#endif