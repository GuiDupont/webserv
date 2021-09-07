/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:58:31 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/07 17:15:39 by gdupont          ###   ########.fr       */
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


#define  WHITESPACE " \t\n\v\f\r"

#define GET 1
#define POST 2
#define DELETE 4

class vHost;

//size_t g_line;

extern size_t g_line;


class webserv {
	
	public:
		webserv(const std::string & path_config);
		webserv();
		~webserv();

	private:
		std::list<vHost> 	vhosts;
		bool				auto_index;
		int					client_max_body_size;
		unknown				error_pages;
		unknown				upload;
		std::string 		root;
		unknown				cgi;
		

	
	private:
		/* PARSING */
		bool	check_brackets(const std::string & config);
		size_t	get_next_closing_bracket(const std::string & config, size_t current);
		bool	check_closing_brackets(const std::string & config);
		void	import_config(std::ifstream infile);
		void	set_config(std::ifstream & config_file);
		void	set_max_body_size(std::string & line);
		void	set_error_page(std::string & line);

};
extern webserv g_webserv;



#endif