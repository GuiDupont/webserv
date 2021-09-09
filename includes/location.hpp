/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 14:50:54 by gdupont          ###   ########.fr       */
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
#include <list>


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
		size_t		_disable_methods; // defined using bitwise operations
		std::list< std::pair<int, std::string> >		_error_pages;
		unknown		_redirection;
		std::string		_upload_pass;
		std::string _root;
		std::string _index;
		unknown		_cgi;
		std::string _path;
};


#endif