/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:58:31 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/22 18:34:16 by ade-garr         ###   ########.fr       */
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




# define WHITESPACE " \t\n\v\f\r"

# define GET 1
# define POST 2
# define DELETE 4


# define TIMEOUT 10

class vHost;
class request;

typedef int unknown;
extern class webserv_parser g_parser;
extern class logger g_logger;

typedef int SOCKET; // a bouger ?
typedef struct sockaddr_in SOCKADDR_IN; // a bouger ?
typedef struct sockaddr SOCKADDR; // a bouger ?


class webserv {
	friend class webserv_parser;
	friend class config;

	public:

	private:
		std::list<vHost> 							_vhosts;
		int											_client_max_body_size;
		std::list< std::pair<int, std::string> >	_error_pages;
		std::string									_upload_pass;
		std::string 								_root;
		std::string									_cgi_dir;
		int											_epfd;
		std::map<int, request>						_requests;
		std::map<int, std::tm>						_timeout;
	public:
		webserv(const std::string & path_config);
		webserv();
		~webserv();

		void	set_config(std::ifstream & config_file);

		void 							set_hosts();
		int								get_epfd() const ;
		std::list<vHost>				&get_vhosts() ;

		void	wait_for_connection();
		void	handle_new_client(int ssock, SOCKADDR* csin, socklen_t* crecsize);
		void	handle_answer_to_request(int csock);


		bool	ft_is_ssock(int fd);
		void	ft_add_csock_to_vhost(int sock, int csock);
		int		get_sock_by_matching_host_ip(std::pair< std::string, size_t> host_port);
		void	display_sock();
		bool	is_pending_request(int csock);
		void	handle_new_request(int csock); // a supprimer ?? (ancienne fonction pour avoir le bdy)
		bool	is_new_request(int fd);
		void	control_time_out(void);
		void	add_event_to_request(int csock);
		void	analyse_header(request &req);
		void	analyse_body(request &req);
		void	set_request_to_ended(request &req);
		void	clean_csock_from_server(int fd);
		bool	is_chunked(request &req);
		int		find_word(std::string str, std::string word);
		bool	is_valid_content_length(std::string val);

	
	private:


};
extern webserv g_webserv;



#endif