/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:02:01 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/10 18:14:48 by gdupont          ###   ########.fr       */
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

class location;

class vHost {

public:

	vHost();
	vHost(std::ifstream &config_file, std::string &line);
	~vHost();
	vHost(vHost const & cpy);

	vHost &operator=(vHost const & rhs);
	// size_t getPort() const ;
	// std::string getHost() const ;
	std::list< std::pair< std::string, size_t> > 		& getHost_Port();
	std::map< int, std::pair< std::string, size_t> > 	& get_sock_list();
	std::list<int> 										& get_csock_list();

	void map_sock_to_hostport(int sock, std::pair< std::string, size_t> host_port);
	size_t setMax() const ;
	// void setHost(std::string host);
	// void setPort(int port);

private:

	std::list<location>									_locations;
	int													_client_max_body_size;
	std::list< std::pair<int, std::string> >			_error_pages;
	std::list< std::pair< std::string, size_t> >		_host_port;
	std::list<std::string>								_server_name;
	std::map< int, std::pair< std::string, size_t> >	_sock_list;
	std::list< int >									_csock_list;
	std::string 										_root;
	
};

#endif