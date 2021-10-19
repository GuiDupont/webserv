/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:11:34 by gdupont          ###   ########.fr       */
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

# define MAX_BODY_SIZE 2000000000

typedef int unknown;
class vHost;
class request;
class location;

class config {

	public:
		std::string									server_name;
		bool										auto_index;
		long long									client_max_body_size;
		std::map< int, std::string >				error_pages;
		std::string									upload_pass;
		std::string 								root;
		std::string									cgi_dir;
		std::string 								index;
		size_t										disable_methods;
		size_t										method;
		std::string									request_target;
		std::map<std::string, std::string>			header_fields;
		std::pair<int, std::string>					return_pair;
		std::pair<std::string, size_t>				host_port;
		bool										return_activated;
		std::set< std::string >						cgi_ext;
		std::string 								location_name;
		std::string									path_to_target;
		int											code;
		bool										validity_checked;
		bool										local_actions_done;
		std::string									query_string;
		std::string									path_info;
		bool										cgi_activated;
		std::string									script_name;

		config(request & req);
		config();

		void							put_vhost_and_location_in_config(vHost & host);
		std::map< std::string, class location >::const_iterator get_most_accurate_location(vHost & host);
		std::string						update_path_to_target_with_root(const class location & location);
		vHost						&	get_associated_vhost(request & request);
		void							set_cgi_params(request & request);
		size_t							get_query_index(const std::string & target, size_t index);

		friend std::ostream & operator<<(std::ostream & o, const config & c);

};




# endif
