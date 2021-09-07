/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/07 17:06:38 by gdupont          ###   ########.fr       */
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
	
	private:
	
		int			_max_body_size;
		int			_disable_methods; // defined using bitwise operations
		unknown		_error_pages;
		unknown		_redirection;
		unknown		_upload;
		std::string _root;
		unknown		_cgi;
		std::string _path;
};


#endif