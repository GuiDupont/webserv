/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/28 14:05:57 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"
# include "vhost.hpp"
# include "request.hpp"
# include "location.hpp"

# include <string>
# include <map>
# include <set>

typedef int unknown;
class vHost;
class request;
class location;

class config {

	public:
		std::string									_server_name;
		bool										_auto_index;
		long										_client_max_body_size;
		std::map< int, std::string >				_error_pages;
		std::string									_upload_pass;
		std::string 								_root;
		std::string									_cgi_dir;
		std::string 								_index;
		size_t										_disable_methods;
		size_t										method;
		std::string									_request_target;
		std::map<std::string, std::string>			_header_fields;
		std::pair<int, std::string>					_return;
		std::pair<std::string, size_t>				host_port;
		bool										return_activated;
		std::set< std::string >						_cgi_ext;
		std::string 								_location;
		std::string									path_to_target;
		int											code;
		bool										validity_checked;
		bool										local_actions_done;
		
	
		config(request & req);
		config();

		void							put_vhost_and_location_in_config(vHost & host, request & conf);
		std::map< std::string, location >::const_iterator get_most_accurate_location(vHost & host);
		std::string						update_path_to_target_with_root(const location & location);
		vHost						&	get_associated_vhost(request & request);


	private:



	public:

		friend std::ostream & operator<<(std::ostream & o, const config & c);

};




# endif
