/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 14:03:10 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/17 13:07:42 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <list>
# include "exceptions.hpp"
# include "webserv.hpp"
# include "vhost.hpp"
# include "request.hpp"

extern size_t g_line;
class webserv;
class vHost;

# define tchar "!#$%&'*+-.^_`|~/"

class webserv_parser {
	private:

	public:
	
		bool								check_brackets(const std::string & config);
		size_t								get_next_closing_bracket(const std::string & config, size_t position);
		bool								check_closing_brackets(const std::string & config);

		size_t								parse_disabled_methods(std::string & line);
		std::string							parse_one_word(std::string & line);
		bool								parse_auto_index(std::string & line);
		void								parse_cgi_extension(std::list< std::string > & cgi_ext, std::string & line);
		size_t								get_max_body_size(std::string & line);
		std::pair<int, std::string> 		parse_return(std::string &line);
		std::pair<int, std::string>			parse_error_page(std::string & line);
		std::string 						parse_server_name(std::string &line);
		void								parse_listen(std::string &line, vHost &host);
		std::string 						parse_upload_pass(std::string &line);
		std::string							parse_ip(std::string str, vHost &host);
		void 								check_server_line(std::string &line);

		std::pair<std::string, std::string> get_header_begin_body(int csock);
		// void								parse_header(std::string & header, request & n_request);
};


#endif