/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/21 19:00:01 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"
# include "vhost.hpp"
# include "request.hpp"

# include <string>
# include <map>
# include <set>

typedef int unknown;
class vHost;
class request;


class config {

	public:
		std::string									_server_name;
		bool										_auto_index;
		int											_client_max_body_size;
		std::list< std::pair<int, std::string> >	_error_pages;
		std::string									_upload_pass;
		std::string 								_root;
		std::string									_cgi_dir;
		std::string 								_index;
		size_t										_disable_methods;
		std::string									_method;
		std::string									_request_target;
		std::map<std::string, std::string>			_header_fields;
		std::pair<int, std::string>					_return;
		std::set< std::string >						_cgi_ext;
		std::string 								_location;
	
		config(request & req);
		void	put_vhost_and_location_in_config(vHost & host, request & conf);
	private:



	public:

		friend std::ostream & operator<<(std::ostream & o, const config & c);

};




# endif
