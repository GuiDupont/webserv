/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:07:25 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/08 10:28:00 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"
# include "config.hpp"
# include "response.hpp"
# include "CGI.hpp"

# include <sys/types.h>
# include <sys/wait.h>
# include <string>
# include <map>
# include <list>
# include <iostream>
# include <fcntl.h>
# include <unistd.h>

# define HDER_REQUEST 0
# define BDY_REQUEST 1
# define ENDED_REQUEST 2

class config;

class CGI;

class request {
    
    public:

		std::string 						method;
		std::string 						request_target;
		std::string 						HTTP_version;
		std::map<std::string, std::string>	header_fields;
		std::list<std::string>				trailer;
		std::string							host_name;
		std::string							body_request;
		std::string							left;
		int									next_chunk; // -2 = trailer stage ; -1 = chunk stage
		int									nb_trailer_to_received;
		int									code_to_send;
		int									csock;
		int									stage; // 0 = new requete, header en cours ; 1 = body en cours ; 2 = requete complete
		config								*conf;
		response							*resp;
		CGI									*cgi;
		bool								close_csock;
		bool								request_with_body;
		bool								body_is_sent;
		bool								header_is_sent;
		int									body_fd;
		size_t								body_written_cgi;
		std::string							body_response;

		friend std::ostream & operator<<(std::ostream & o,const request & r);
		
    public:

		request(int csock);
		request(int csock, std::string left);
		request();
		~request();
		
		void	response_request();
		void	control_config_validity();
		bool	common_validity_check();
		void	param_trailer(std::string str);
		bool	is_valid_chunk_size(std::string &str);
		bool	find_trailer_in_list(std::string str);
		bool	header_is_not_sent();
		void	generate_header();
		void	send_header(int csock, std::string & header);
		void	send_body();
		void	set_request_to_ended();
		void	send_body_from_str();
		void	send_body_from_file();
		void	do_local_actions();
		void	update_code_and_body();
		void	delete_directory(std::string & path, request & req);
		void	delete_file(std::string & path, request & req);
		void	write_body_inside_file();
		void	initiate_CGI();
		void	initiate_CGI_GET();
		void	initiate_CGI_POST();
		void	readfrom_CGI();
		void	read_first_line_cgi();
		void	erase_static_fd_from_request(int fd);


};



#endif