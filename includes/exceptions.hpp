/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:09:27 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/07 14:31:07 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

class config_file_error : public std::exception {
	public:
		virtual const char* what() const throw();
};

class config_file_not_open : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_brackets_conf : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_directive : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_client_max_body_size : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_location_declaration : public std::exception {
	public:
		virtual const char* what() const throw();
};

#endif