/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:20:01 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

# include <string>
# include <map>
# include <fstream>
# include <list>

# include "vhost.hpp"
# include "utils.hpp"
# include "webserv.hpp"
# include "exceptions.hpp"
# include "location.hpp"
# include "insert_map_in_to_map.tpp"

class vHost;
class location {

	public:
		location() ;
		location(std::ifstream & config_file, std::string & line, vHost & vhost);

		std::string	const									& get_location_name() const;
		int			const									& get_client_max_body_size() const;
		bool		const									& get_auto_index() const;
		size_t		const									& get_disable_methods() const;
		std::map< int, std::string >				const	& get_error_pages() const;
		std::string	const									& get_upload_pass() const;
		std::string const									& get_root() const;
		std::string const									& get_index() const;
		std::pair<int, std::string>					const	& get_return() const;
		std::set< std::string >	const						& get_cgi_ext() const;
		std::string 								const   & get_cgi_dir() const;

	private:

		int												_client_max_body_size;
		bool											_auto_index;
		size_t											_disable_methods;
		std::map< int, std::string >					_error_pages;
		std::string										_upload_pass;
		std::string 									_root;
		std::string 									_index;
		std::pair<int, std::string>						_return_pair;
		std::set< std::string >							_cgi_ext;
		std::string 									_location_name;
		std::string										_cgi_dir;
};


#endif