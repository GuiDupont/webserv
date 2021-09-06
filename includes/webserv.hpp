/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:58:31 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/06 17:36:02 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vhost.hpp"
#include <list>
#include <string>
//#include <sys/epoll.h>


class webserv {
	
	public:
		webserv(const std::string & path_config);
		webserv();
		~webserv();

	private:
		std::list<vHost> 	vhosts;
		bool				auto_index;
		int					max_body_size;
		unknown				error_pages;
		unknown				upload;
		std::string 		root;
		unknown				cgi;
		

	
	private:
		/* PARSING */
		bool	check_brackets(const std::string & config);
		size_t	get_next_closing_bracket(const std::string & config, size_t current);
		bool	check_closing_brackets(const std::string & config);



};
