/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/23 12:40:21 by gdupont          ###   ########.fr       */
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

class vHost;
class location {

	public:
		location() ;
		location(std::ifstream & config_file, std::string & line, vHost & vhost);

		std::string	const									& get_location() const;
		int			const									& get_client_max_body_size() const;
		bool		const									& get_auto_index() const;
		size_t		const									& get_disable_methods() const;
		std::list< std::pair<int, std::string> >	const	& get_error_pages() const;
		std::string	const									& get_upload_pass() const;
		std::string const									& get_root() const;
		std::string const									& get_index() const;
		std::pair<int, std::string>					const	& get_return() const;
		std::set< std::string >	const						& get_cgi_ext() const;

	private:

		int												_client_max_body_size;
		bool											_auto_index;
		size_t											_disable_methods;
		std::list< std::pair<int, std::string> >		_error_pages;
		std::string										_upload_pass;
		std::string 									_root;
		std::string 									_index;
		std::pair<int, std::string>						_return;
		std::set< std::string >							_cgi_ext;
		std::string 									_location;
		std::string										_cgi_dir;
};


#endif