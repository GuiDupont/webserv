/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/12 15:17:41 by gdupont          ###   ########.fr       */
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

	private:

		int												_client_max_body_size;
		bool											_auto_index;
		size_t											_disable_methods; // defined using bitwise operations
		std::list< std::pair<int, std::string> >		_error_pages;
		std::string										_upload_pass;
		std::string 									_root;
		std::string 									_index;
		std::list< std::pair<int, std::string> >		_return;
		std::list< std::string >						_cgi_ext;
		std::string 									_path;
};


#endif