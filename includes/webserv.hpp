/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:58:31 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 16:32:09 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "vhost.hpp"
#include <list>
#include <string>
#include "utils.hpp"
#include <cstdlib>

//#include <sys/epoll.h>


#define WHITESPACE " \t\n\v\f\r"

#define GET 1
#define POST 2
#define DELETE 4

class vHost;

extern size_t g_line;

typedef int unknown;

class webserv {
	
	public:
		webserv(const std::string & path_config);
		webserv();
		~webserv();

		void set_hosts();

	private:
		std::list<vHost> 							vhosts;
		bool										auto_index;
		int											_client_max_body_size;
		std::list< std::pair<int, std::string> >	_error_pages;
		std::string									upload;
		std::string 		root;
		unknown				cgi;
		int											_epfd;
	
	private:
		/* PARSING */
		bool	check_brackets(const std::string & config);
		size_t	get_next_closing_bracket(const std::string & config, size_t current);
		bool	check_closing_brackets(const std::string & config);
		void	import_config(std::ifstream infile);
		void	set_config(std::ifstream & config_file);
		void	set_error_page(std::string & line);

};
extern webserv g_webserv;



#endif