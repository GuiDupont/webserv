/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vhost.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:02:01 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/06 14:48:21 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include <list>


class vHost {

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
};