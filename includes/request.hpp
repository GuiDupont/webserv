/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:07:25 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/23 18:30:49 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"
# include "config.hpp"
# include "response.hpp"



# include <string>
# include <map>
# include <list>
# include <iostream>

# define HDER_REQUEST 0
# define BDY_REQUEST 1
# define ENDED_REQUEST 2

class config;

class request {
    
    public:

		std::string 						_method;
		std::string 						_request_target;
		std::string 						_HTTP_version;
		std::map<std::string, std::string>	_header_fields;
		std::list<std::string>				_trailer;
		std::string							_host;
		std::string							_body;
		std::string							_left;
		int									next_chunk; // -2 = trailer stage ; -1 = chunk stage
		int									nb_trailer_to_received;
		int									_code_to_send;
		int									_csock;
		int									stage; // 0 = new requete, header en cours ; 1 = body en cours ; 2 = requete complete
		config								*conf;
		response							*resp;
		bool								done;
		bool								local_actions_done;
		bool								close_csock;
		friend std::ostream & operator<<(std::ostream & o,const request & r);
		
    public:

		request(int csock);
		request(int csock, std::string left);
		request();
		~request();
		
		void	response_request();
		void	control_config_validity();
		void	param_trailer(std::string str);
		bool	is_valid_chunk_size(std::string &str);
		bool	find_trailer_in_list(std::string str);
		void	do_local_actions();
		bool	header_is_not_sent();
		void	generate_header();
		void	send_header();
		void	send_body();
		
		// ~request();
		// request & operator=()
};



#endif