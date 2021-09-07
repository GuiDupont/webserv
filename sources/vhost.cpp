/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/07 11:18:29 by ade-garr          #+#    #+#             */
/*   Updated: 2021/09/07 11:26:56 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vhost.hpp"

vHost::vHost() {

	return ;
}

vHost::~vHost {

	return ;
}

vHost::vHost(vHost const & cpy) {

	*this = cpy;
	return ;
}

vHost & vHost::operator=(vHost const & rhs) {

	this->locations = rhs.locations;
	this->auto_index = auto_index;
	this->max_body_size = rhs.max_body_size;
	this->allowed_methods = rhs.allowed_methods;
	this->error_pages = rhs.error_pages;
	this->redirection = rhs.redirection;
	this->upload = rhs.upload;
	this->host = rhs.host;
	this->port = rhs.port;
	this->server_name = rhs.server_name;
	this->root = rhs.root;
	this->cgi = rhs.cgi;
	return (*this);
}