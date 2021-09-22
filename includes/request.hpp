/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:07:25 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/22 17:56:34 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"
# include "config.hpp"


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
		int									next_chunk;
		int									_code_to_send;
		int									_csock;
		int									stage; // 0 = new requete, header en cours ; 1 = body en cours ; 2 = requete complete
		config								*config;
		friend std::ostream & operator<<(std::ostream & o,const request & r);
		
    public:

		request(int csock);
		request(int csock, std::string left);
		request();

		void	control_validity();
		void	param_trailer(request &req, std::string str);
		bool	is_valid_chunk_size(std::string &str);
		
		// ~request();
		// request & operator=()
};



#endif