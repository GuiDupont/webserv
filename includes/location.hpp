/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 15:46:48 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/07 13:48:10 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GET 1
#define POST 2
#define DELETE 4

typedef int unknown;

#include <string>

struct location {
	int			max_body_size;
	int			allowed_methods; // defined using bitwise operations
	unknown		error_pages;
	unknown		redirection;
	unknown		upload;
	std::string root;
	unknown		cgi;
};