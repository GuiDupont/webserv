/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:02:01 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 17:47:22 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VHOST_H
# define VHOST_H

# include "config.hpp"
# include "location.hpp"
# include <list>
#include <utility>

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
	size_t getPort() const ;
	std::string getHost() const ;
	std::list< std::pair< std::string, size_t> >  getHost_Port() const ;
	size_t setMax() const ;
	void setHost(std::string host);
	void setPort(int port);

private:

	std::list<location>							_locations;
	bool										_auto_index;
	int											_client_max_body_size;
	int											_allowed_methods; // defined using bitwise operations
	std::list< std::pair<int, std::string> >	_error_pages;
	std::list< std::pair<int, std::string> >	_return;
	std::string									_upload_pass;
	std::list< std::pair< std::string, size_t> >				_host_port;
	std::string 								_host;
	size_t										_port;
	std::string 								_server_name;
	std::string 								_root;
	unknown										_cgi;
};

#endif