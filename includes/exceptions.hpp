/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:09:27 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/09 13:47:09 by gdupont          ###   ########.fr       */
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


class non_existing_disabled_methods : public std::exception {
	public:
		virtual const char* what() const throw();
};

class empty_declaration : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_auto_index_value : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_error_page_value : public std::exception {
	public:
		virtual const char* what() const throw();
};

class empty_error_page_path : public std::exception {
	public:
		virtual const char* what() const throw();
};

class bad_ip_address : public std::exception {

	public:

		virtual const char* what() const throw();
};

class bad_port : public std::exception {
	public:
		virtual const char* what() const throw();
};

#endif
