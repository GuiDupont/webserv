/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:02:01 by gdupont           #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2021/09/07 16:07:22 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VHOST_H
# define VHOST_H

# include "config.hpp"
# include <list>
# include <fstream>
# include <string>

class vHost {

public:

	vHost();
	vHost(std::ifstream &config_file, std::string &line);
	~vHost();
	vHost(vHost const & cpy);

	vHost &operator=(vHost const & rhs);
	size_t getPort() const ;
	size_t setMax() const ;

private:
	std::list<location> locations;

	bool		auto_index;
	int			max_body_size;
	int			allowed_methods; // defined using bitwise operations
	unknown		error_pages;
	unknown		redirection;
	unknown		upload;
	std::string host;
	size_t		port;
	std::string server_name;
	std::string root;
	unknown		cgi;
=======
/*   Updated: 2021/09/07 16:23:05 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VHOST_HPP
#define VHOST_HPP

#include "location.hpp"
#include <list>

class location;

class vHost {

	public:

		vHost();
		~vHost();
		vHost(vHost const & cpy);

		vHost &operator=(vHost const & rhs);


	private:
		std::list<location> locations;

		bool		auto_index;
		int			max_body_size;
		int			allowed_methods; // defined using bitwise operations
		unknown		error_pages;
		unknown		redirection;
		unknown		upload;
		std::string host;
		size_t		port;
		std::string server_name;
		std::string root;
		unknown		cgi;
	
	
>>>>>>> config
};

#endif