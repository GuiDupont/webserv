/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 15:34:17 by ade-garr          #+#    #+#             */
/*   Updated: 2021/10/21 10:25:53 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(request & req) : status_read(false) {

    for (int i = 0; i < 15; i++) {
	    env[i] = 0;
    }
	setCgi_stage("WRITEIN");
	param_CONTENT_LENGTH(req);
	param_CONTENT_TYPE(req);
	param_GATEWAY_INTERFACE();
	param_PATH_INFO(req);
	param_PATH_TRANSLATED(req);
	param_QUERY_STRING(req);
	param_REMOTE_ADDR();
	param_REQUEST_METHOD(req);
	param_SCRIPT_NAME(req);
	param_SERVER_NAME();
	param_SERVER_PORT();
	param_SERVER_PROTOCOL();
	param_SERVER_SOFTWARE();
	param_REDIRECT_STATUS();
}

CGI::CGI() : status_read(false), cgi_stage("WRITEIN") { }
CGI::~CGI() { }

void clear_fork_post(request & req) {
	g_logger.fd << g_logger.get_timestamp() << "The fork for POST didn't work\n";
	req.code_to_send = 500;
	req.conf->local_actions_done = true;
	req.close_csock = true;
	close(req.cgi->pipefd[0]);
	close(req.cgi->pipefd[1]);
	close(req.cgi->pipefd_post[0]);
	close(req.cgi->pipefd_post[1]);
}

void clear_fork_get(request & req) {
	g_logger.fd << g_logger.get_timestamp() << "The fork didn't work\n";
	req.code_to_send = 500;
	req.conf->local_actions_done = true;
	req.close_csock = true;		
	close(req.cgi->pipefd[0]);
	close(req.cgi->pipefd[1]);
	return ;
}

void clear_second_pipe_post(request & req) {
	g_logger.fd << g_logger.get_timestamp() << "The 2nd pipe for POST didn't work\n";
	req.code_to_send = 500;
	req.conf->local_actions_done = true;
	req.close_csock = true;
	close(req.cgi->pipefd[0]);
	close(req.cgi->pipefd[1]);
}

void clear_first_pipe_post(request & req) {
	g_logger.fd << g_logger.get_timestamp() << "The 1ST pipe for POST didn't work\n";
	req.code_to_send = 500;
	req.conf->local_actions_done = true;
	req.close_csock = true;
}

void cgi_child_post(request & req) {
	int ret;
	close(req.cgi->pipefd[0]);
	ret = dup2(req.cgi->pipefd[1], 1);
	if (ret == -1)
		exit (1);
	close(req.cgi->pipefd[1]);
	close(req.cgi->pipefd_post[1]);
	ret = dup2(req.cgi->pipefd_post[0], 0);
	if (ret == -1)
		exit (1);
	close(req.cgi->pipefd_post[0]);
	char *arg2[] = {(char *)req.conf->cgi_dir.c_str(), (char *)req.conf->path_to_target.c_str(), NULL};
	ret = execve(req.conf->cgi_dir.c_str(), arg2, req.cgi->getenv());
	if (ret == -1)
		exit(1);
}

void cgi_child_get(request & req) {
	int ret;
	char *arg2[] = {(char *)req.conf->cgi_dir.c_str(), (char *)req.conf->path_to_target.c_str(), NULL};
	close(req.cgi->pipefd[0]);
	ret = dup2(req.cgi->pipefd[1], 1);
	if (ret == -1)
		exit(1);
	close(req.cgi->pipefd[1]);
	
	ret = execve(req.conf->cgi_dir.c_str(), arg2, req.cgi->getenv());
	if (ret == -1)
		exit(1);
}

void	cgi_father_get(request & req) {

	close(req.cgi->pipefd[1]);	
	waitpid(req.cgi->pid, &req.cgi->pid_status, WNOHANG);
	if (WIFEXITED(req.cgi->pid_status) == true && WEXITSTATUS(req.cgi->pid_status) != 0) {
		req.code_to_send = 500;
		req.conf->local_actions_done = true;
		req.close_csock = true;
		close(req.cgi->pipefd[0]);
		return ;
	}
	add_fd_epollin_to_pool(req.cgi->pipefd[0]);
	g_webserv.static_fds.insert(req.cgi->pipefd[0]);
	req.cgi->setCgi_stage("READFROM");
	g_logger.fd << g_logger.get_timestamp() << "CGI GET is done setting up we are waiting for cgi" << std::endl;
}

void	cgi_father_post(request & req) {
	close(req.cgi->pipefd[1]);
	close(req.cgi->pipefd_post[0]);
	waitpid(req.cgi->pid, &req.cgi->pid_status, WNOHANG);
	if (WIFEXITED(req.cgi->pid_status) == true && WEXITSTATUS(req.cgi->pid_status) != 0) {
		req.code_to_send = 500;
		req.conf->local_actions_done = true;
		req.close_csock = true;
		close(req.cgi->pipefd[0]);
		close(req.cgi->pipefd_post[1]);
		return ;
	}
	add_fd_epollin_to_pool(req.cgi->pipefd[0]);
	g_webserv.static_fds.insert(req.cgi->pipefd[0]);
	add_fd_epollout_to_pool(req.cgi->pipefd_post[1]);
	g_webserv.static_fds.insert(req.cgi->pipefd_post[1]);
}



void	clear_pipe_get(request & req) {
	g_logger.fd << g_logger.get_timestamp() << "The pipe didn't work\n";
	req.code_to_send = 500;
	req.conf->local_actions_done = true;
	req.close_csock = true;
}

void request::initiate_CGI_GET() {
	g_logger.fd << g_logger.get_timestamp() << "We are going to set up a CGI for a GET for " << csock << std::endl;

	int ret = pipe(cgi->pipefd);
	if (ret == -1)
		clear_pipe_get(*this);
	cgi->pid = fork();
	if (cgi->pid == -1) 
		return (clear_fork_get(*this));

	if (cgi->pid == 0)
		cgi_child_get(*this);
	if (cgi->pid != 0)
		cgi_father_get(*this);
}

void request::initiate_CGI_POST() {

	int ret = pipe(cgi->pipefd);
	fcntl(cgi->pipefd[0], F_SETFL, O_NONBLOCK);
	if (ret == -1)
		return clear_first_pipe_post(*this);
	
	ret = pipe(cgi->pipefd_post);
	if (ret == -1)
		return clear_second_pipe_post(*this);
	
	cgi->pid = fork();
	if (cgi->pid == -1)
		return clear_fork_post(*this);

	if (cgi->pid == 0)
		cgi_child_post(*this);
	if (cgi->pid != 0)
		cgi_father_post(*this);
}

void request::handle_CGI() {

	char buf[SEND_SPEED + 1];
	int ret;

	g_logger.fd << g_logger.get_timestamp() << "We are about to handle a CGI request\n";
	if (cgi == NULL) {
		cgi = new CGI(*this);
		if (method == "GET")
			initiate_CGI_GET();
		if (method == "POST")
			initiate_CGI_POST();
	}
	else if (this->cgi->getCgi_stage() == "WRITEIN") {
		waitpid(cgi->pid, &cgi->pid_status, WNOHANG);
		if (WIFEXITED(cgi->pid_status) == true && WEXITSTATUS(cgi->pid_status) != 0) {
			g_logger.fd << g_logger.get_timestamp() << "The child exited with an error\n";
			code_to_send = 500;
			close_csock = true;
			conf->local_actions_done = true;
			erase_static_fd_from_request(cgi->pipefd[0]);
			close(cgi->pipefd[0]);
			g_webserv.static_fds_to_close.insert(cgi->pipefd_post[1]);
			return ;
		}
		std::string towrite = body_request.substr(body_written_cgi, SEND_SPEED);
		if (can_I_write_in_fd(cgi->pipefd_post[1]) == false)
			return ;
		ret = write(cgi->pipefd_post[1], towrite.c_str(), towrite.size());
		if (ret == -1) {
			g_logger.fd << g_logger.get_timestamp() << "Couldn't write on pipe CGI\n";
			code_to_send = 500;
			close_csock = true;
			conf->local_actions_done = true;
			erase_static_fd_from_request(cgi->pipefd[0]);
			close(cgi->pipefd[0]);
			g_webserv.static_fds_to_close.insert(cgi->pipefd_post[1]);
		}
		body_written_cgi += ret;
		if (body_written_cgi == body_request.size() || ret == 0) {
			if (ret == 0)
				g_logger.fd << g_logger.get_timestamp() << "ret == 0 pour un cgi !!!!!!!!!\n";
			g_webserv.static_fds_to_close.insert(cgi->pipefd_post[1]);
			cgi->setCgi_stage("READFROM");
		}
	}
	else if (this->cgi->getCgi_stage() == "READFROM") {
		waitpid(cgi->pid, &cgi->pid_status, WNOHANG);
		if (child_exited_badly())
			return ;
		if (can_I_read_from_fd(cgi->pipefd[0]) == true) {
			ret = read(cgi->pipefd[0], buf, SEND_SPEED);
			if (ret == -1) {
				close_csock = true;
				code_to_send = 500;
				erase_static_fd_from_request(cgi->pipefd[0]);
				close(cgi->pipefd[0]);
				conf->cgi_activated = false;
				conf->local_actions_done = true;
				return ;
			}
			else {
				buf[ret] = '\0';
				body_response += buf;
				if (ret == 0) {
					erase_static_fd_from_request(cgi->pipefd[0]);
					close(cgi->pipefd[0]);
					conf->local_actions_done = true;
				}
			}
		}
		else if (is_EPOLLHUP(cgi->pipefd[0]) == true) {
			erase_static_fd_from_request(cgi->pipefd[0]);
			close(cgi->pipefd[0]);
			webserv_parser::parse_cgi_body_response(*this);
			conf->local_actions_done = true;
		}
	}
}

bool request::child_exited_badly() {
	if (WIFEXITED(cgi->pid_status) == true && WEXITSTATUS(cgi->pid_status) != 0) {
		g_logger.fd << g_logger.get_timestamp() << "Child exited badly\n";
		code_to_send = 500;
		conf->cgi_activated = false;
		close_csock = true;
		erase_static_fd_from_request(cgi->pipefd[0]);
		close(cgi->pipefd[0]);
		return (true);
	}
	return false;
}

char **CGI::getenv() {

	return (this->env);
}
void CGI::param_REDIRECT_STATUS() {

	this->REDIRECT_STATUS = "REDIRECT_STATUS=200";
	this->env[13] = (char *)this->REDIRECT_STATUS.c_str();
	return ;
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
		if (req.header_fields.find("content-type") != req.header_fields.end())
			this->CONTENT_TYPE = "CONTENT_TYPE=" + req.header_fields.find("content-type")->second;
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

std::string &CGI::getCgi_stage() {

	return (this->cgi_stage);
}
