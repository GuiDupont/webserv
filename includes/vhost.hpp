/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:02:01 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 12:52:58 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VHOST_H
# define VHOST_H

# include "config.hpp"
# include "location.hpp"
# include <list>
# include <pair>
# include <fstream>
# include <string>

class location;
class pair;

class vHost {

public:

	vHost();
	vHost(std::ifstream &config_file, std::string &line);
	~vHost();
	vHost(vHost const & cpy);

	vHost &operator=(vHost const & rhs);
	size_t getPort() const ;
	size_t setMax() const ;
	void setHost(std::string host);
	void setPort(int port);

private:

	std::list<location> locations;
	bool		auto_index;
	int			_client_max_body_size;
	int			allowed_methods; // defined using bitwise operations
	std::list<std::pair<int, std::string>>	error_pages;
	std::list<std::pair<int, std::string>>	_return;
	std::string		upload_pass;
	std::string host;
	size_t		port;
	std::string server_name;
	std::string root;
	unknown		cgi;
};

#endif