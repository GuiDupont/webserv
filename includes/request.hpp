/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:07:25 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/20 12:34:35 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"

# include <string>
# include <map>
# include <iostream>

# define HDER_REQUEST 0
# define BDY_REQUEST 1
# define ENDED_REQUEST 2

class request {
    
    public:

		std::string 						_method;
		std::string 						_request_target;
		std::string 						_HTTP_version;
		std::map<std::string, std::string>	_header_fields;
		std::string							_body;
		std::string							_left;
		int									_error_to_send;
		int									_csock;
		int									stage; // 0 = new requete, header en cours ; 1 = body en cours ; 2 = requete complete
		friend std::ostream & operator<<(std::ostream & o,const request & r);
		
    public:

		request(std::string & header); // a supprimer ?? (ancienne fonction pour avoir le bdy)
		request();
		// ~request();
		// request & operator=()
};



#endif