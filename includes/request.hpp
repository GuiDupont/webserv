/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:07:25 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/17 13:03:55 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "utils.hpp"

# include <string>
# include <map>
# include <iostream>


class request {
    
    public:

		std::string 						_method;
		std::string 						_request_target;
		std::string 						_HTTP_version;
		std::map<std::string, std::string>	_header_fields;
		std::string							_body;
		int									_error_to_send;
		int									_csock;
		
		friend std::ostream & operator<<(std::ostream & o,const request & r);
		
    public:

		request(std::string & header);
		request();
		// ~request();
		// request & operator=()
};



#endif