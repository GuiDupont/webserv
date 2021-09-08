/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/08 14:53:26 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

typedef int unknown;

#include <string>
#include "vhost.hpp"
#include "utils.hpp"
#include "webserv.hpp"
#include "exceptions.hpp"
#include "location.hpp"
#include <fstream>


class location {

	public:
		location();
		location(std::ifstream & config_file, std::string & line);
		
		size_t			parse_disabled_methods(std::string & line);
		std::string		parse_root(std::string & line);
		bool			parse_auto_index(std::string & line);

	
		
	private:
	
		int			_client_max_body_size;
		bool		_auto_index;
		size_t		_disabled_methods; // defined using bitwise operations
		unknown		_error_pages;
		unknown		_redirection;
		unknown		_upload;
		std::string _root;
		unknown		_cgi;
		std::string _path;
};


#endif