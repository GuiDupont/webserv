/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:09:27 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/08 22:47:43 by ade-garr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

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

class no_port_associated : public std::exception {

public:

	virtual const char* what() const throw();
};

class bad_nb_argument : public std::exception {

public:

	bad_nb_argument(std::string directive);
	virtual ~bad_nb_argument() throw () {}
	
	virtual const char* what() const throw();
	virtual std::string obj();

private:

	std::string _directive;
};

class bad_location_declaration : public std::exception {
	public:
		virtual const char* what() const throw();
};

#endif
