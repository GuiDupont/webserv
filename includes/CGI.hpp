/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 15:32:44 by ade-garr          #+#    #+#             */
/*   Updated: 2021/10/11 14:37:32 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include "request.hpp"

class request;

class CGI {

public:

	CGI();
	CGI(request & req);
	~CGI();
	
	void param_CONTENT_LENGTH(request &req);
	void param_CONTENT_TYPE(request &req);
	void param_GATEWAY_INTERFACE();
	void param_PATH_INFO(request &req);
	void param_PATH_TRANSLATED(request &req);
	void param_QUERY_STRING(request &req);
	void param_REMOTE_ADDR();
	void param_REQUEST_METHOD(request &req);
	void param_SCRIPT_NAME(request &req);
	void param_SERVER_NAME();
	void param_SERVER_PORT();
	void param_SERVER_PROTOCOL();
	void param_SERVER_SOFTWARE();
	void param_REDIRECT_STATUS();
	char **getenv();
	void setCgi_stage(std::string s);
	std::string &getCgi_stage();

	int pipefd[2];
	int pipefd_post[2];
	pid_t pid;
	int	pid_status;
	bool status_read;
	std::string first_line;
	std::string left_from_first_line;
	std::string cgi_header_fields;


private:

	std::string CONTENT_LENGTH; // seulement si bdy
	std::string CONTENT_TYPE; // seulement si present dans client request
	std::string GATEWAY_INTERFACE; // = "GATEWAY_INTERFACE=CGI/1.1";
	std::string PATH_INFO; // = ce qui suit le nom du script/fichier
	std::string PATH_TRANSLATED; // = transformation in server document
	std::string QUERY_STRING; // = ce qui suit le '?' ou bien ""
	std::string REMOTE_ADDR; // obligatoire ? = host
	std::string REQUEST_METHOD; // GET ou POST
	std::string SCRIPT_NAME; // path to script
	std::string SERVER_NAME; // = "SERVER_NAME=42webserv";
	std::string SERVER_PORT;
	std::string SERVER_PROTOCOL; // = "SERVER_PROTOCOL=HTTP/1.1";
	std::string SERVER_SOFTWARE; // = "SERVER_SOFTWARE=42webserv";
	std::string REDIRECT_STATUS; // = "REDIRECT_STATUS=200";  => a voir
	char		*env[15];
	std::string	cgi_stage; // "writein" ; "readfrom"
	

};

#endif