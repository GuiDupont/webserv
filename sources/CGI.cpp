/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 15:34:17 by ade-garr          #+#    #+#             */
/*   Updated: 2021/10/06 18:46:52 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI() {

    for (int i = 0; i < 14; i++) {
	    env[i] = 0;
    }
    return ;
}

CGI::~CGI() {

    return ;
}

char **CGI::getenv() {

	return (this->env);
}

void CGI::param_SERVER_SOFTWARE() {

	this->SERVER_SOFTWARE = "SERVER_SOFTWARE=42webserv";
	this->env[12] = (char *)this->SERVER_SOFTWARE.c_str();
	return ;
}

void CGI::param_SERVER_PROTOCOL() {

	this->SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1";
	this->env[11] = (char *)this->SERVER_PROTOCOL.c_str();
	return ;
}

void CGI::param_SERVER_PORT() {

	this->SERVER_PORT = "SERVER_PORT=0";
	this->env[10] = (char *)this->SERVER_PORT.c_str();
	return ;
}

void CGI::param_SERVER_NAME() {

	this->SERVER_NAME = "SERVER_NAME=42webserv";
	this->env[9] = (char *)this->SERVER_NAME.c_str();
	return ;
}

void CGI::param_SCRIPT_NAME(request &req) {

	this->SCRIPT_NAME = "SCRIPT_NAME=" + req.conf->script_name;
	this->env[8] = (char *)this->SCRIPT_NAME.c_str();
	return ;
}

void CGI::param_REQUEST_METHOD(request &req) {

	this->REQUEST_METHOD = "REQUEST_METHOD=" + req.method;
	this->env[7] = (char *)this->REQUEST_METHOD.c_str();
	return ;
}

void CGI::param_REMOTE_ADDR() {

	this->REMOTE_ADDR = "REMOTE_ADDR=0.0.0.0";
	this->env[6] = (char *)this->REMOTE_ADDR.c_str();
	return ;
}


void CGI::param_QUERY_STRING(request &req) {

	this->QUERY_STRING = "QUERY_STRING=" + req.conf->query_string;
	this->env[5] = (char *)this->QUERY_STRING.c_str();
	return ;
}

void CGI::param_PATH_TRANSLATED(request &req) {

	this->PATH_TRANSLATED = "PATH_TRANSLATED=" + req.conf->path_to_target;
	this->env[4] = (char *)this->PATH_TRANSLATED.c_str();
	return ;
}

void CGI::param_PATH_INFO(request &req) {

	this->PATH_INFO = "PATH_INFO=" + req.conf->path_info;
	this->env[3] = (char *)this->PATH_INFO.c_str();
	return ;
}

void	CGI::param_GATEWAY_INTERFACE() {

	this->GATEWAY_INTERFACE = "GATEWAY_INTERFACE=CGI/1.1";
	this->env[2] = (char *)this->GATEWAY_INTERFACE.c_str();
	return ;
}

void	CGI::param_CONTENT_TYPE(request &req) {

	if (req.method == "GET")
		this->CONTENT_TYPE = "CONTENT_TYPE=";
	else {
		if (req.header_fields.find("Content-Type") != req.header_fields.end())
			this->CONTENT_TYPE = "CONTENT_TYPE=" + req.header_fields.find("Content-Type")->second;
		else
			this->CONTENT_TYPE = "CONTENT_TYPE=";
	}
	this->env[1] = (char *)this->CONTENT_TYPE.c_str();
	return ;
}

void	CGI::param_CONTENT_LENGTH(request &req) {

	if (req.method == "GET")
		this->CONTENT_LENGTH = "CONTENT_LENGTH=";
	else
		this->CONTENT_LENGTH = "CONTENT_LENGTH=" + ft_itos(req.body_request.size());
	this->env[0] = (char *)this->CONTENT_LENGTH.c_str();
	return ;
}

void CGI::setCgi_stage(std::string s) {
	
	cgi_stage = s;
	return ;
}