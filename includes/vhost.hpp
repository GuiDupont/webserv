/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:02:01 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:27:42 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VHOST_H
# define VHOST_H

# include "config.hpp"
# include "location.hpp"
# include <list>
# include <utility>
# include <map>
# include <fstream>
# include <string>
# include <set>

class location;

class vHost {

public:

	friend class config;

	vHost();
	vHost(std::ifstream &config_file, std::string &line);
	~vHost();
	vHost(vHost const & cpy);

	vHost &operator=(vHost const & rhs);
	std::list< std::pair< std::string, size_t> > 		& getHost_Port();
	std::map< int, std::pair< std::string, size_t> > 	& get_sock_list();
	std::set<int> 										& get_csock_list();
	std::set<std::string> 								& get_server_names();
	int	const											& get_client_max_body_size() const;
	std::string	const									& get_root() const;

	void 			map_sock_to_hostport(int sock, std::pair< std::string, size_t> host_port);
	size_t 			setMax() const;
	
	std::map< int, std::string > const	& get_error_pages() const;
	std::map< std::string, location >	const	& get_locations() const;

private:

	std::map<std::string, location>						_locations;
	int													_client_max_body_size;
	std::map< int, std::string >						_error_pages;
	std::list< std::pair< std::string, size_t> >		_host_port;
	std::set<std::string>								_server_name;
	std::map< int, std::pair< std::string, size_t> >	_sock_list;
	std::set<int>										_csock_list;
	std::string 										_root;
};

#endif