/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 14:06:41 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/08 12:33:47 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"


std::ostream & operator<<(std::ostream & o,const request & r)
{
	o << "Methode: " << r.method << std::endl;
	o << "Request_target: " << r.request_target << std::endl;
	std::map<std::string, std::string>::const_iterator it = r.header_fields.begin();
	for (; it != r.header_fields.end(); it++)
	{
		o << "Header field : -" << it->first << "-" << std::endl;
		o << "Field content: -" << it->second << std::endl;
	}
	o << "Body: -" << r.body_request << "-" << std::endl;
	return (o);
}


request::request() : stage(0), next_chunk(-1), nb_trailer_to_received(0), code_to_send(0), close_csock(false), conf(NULL), resp(NULL), cgi(NULL), header_is_sent(false), body_is_sent(false), body_fd(-1), body_written_cgi(0) {}

request::request(int csock) :	stage(0), csock(csock), next_chunk(-1), nb_trailer_to_received(0), 
								code_to_send(0), close_csock(false), conf(NULL), resp(NULL), cgi(NULL),
								header_is_sent(false), body_is_sent(false), body_fd(-1), body_written_cgi(0) {}

request::request(int csock, std::string left) : stage(0), csock(csock), left(left), next_chunk(-1),
												nb_trailer_to_received(0), code_to_send(0), close_csock(false),
												conf(NULL), resp(NULL), cgi(NULL), header_is_sent(false), body_is_sent(false), body_fd(-1), body_written_cgi(0) {}


void request::param_trailer(std::string str) {

	std::string::iterator it = str.begin();
	std::string::iterator it2;
	std::string::iterator it3;
	std::string substr;

	if (str.size() == 0)
		return ;
	for (; it != str.end(); it++) {
		for (; it != str.end() && std::isspace(*it) != 0; it++) {}
		if (str.find(',', it - str.begin()) != std::string::npos)
			it2 = str.begin() + str.find(',', it - str.begin());
		else
			it2 = str.end();
		substr = str.substr(it - str.begin(), it2 - it);
		it3 = it2 - 1;
		for (; it3 - it >= 0 && std::isspace(*it3) != 0; it3--) {}
		substr = substr.substr(0, it3 - it + 1);
		this->trailer.push_back(substr);
		it = (it2 == str.end() ? it2 - 1 : it2);
	}
	this->trailer.unique();
	this->nb_trailer_to_received = this->trailer.size();
	return ;
}

bool request::is_valid_chunk_size(std::string &str) {

	regex_t	regex;
	int 	reti;

	reti = regcomp(&regex, "^[0-9a-fA-F]\\{1,8\\}$", 0);
    reti = regexec(&regex, str.c_str(), 0, NULL, 0);
	if (reti) {
    	regfree(&regex);
		return (0);
    }
    regfree(&regex);
	return (1);
}

bool request::find_trailer_in_list(std::string str) {

	for (std::list<std::string>::iterator it = this->trailer.begin(); it != this->trailer.end(); it++) {
		if (str == *it)
			return (1);
	}
	return (0);
}

void	request::send_header(int csock, std::string & header) {
	send(csock, header.c_str(), header.size(), 0);
	std::cout << "Header sent: -" << header.c_str() << "-" << std::endl;
	g_logger.fd << g_logger.get_timestamp() << "We just sent an header" << std::endl;
}

void	request::send_body() {
	if (body_response.empty() == false)
		send_body_from_str();
	else if (method != "POST")
		send_body_from_file();
	else
		body_is_sent = true;
}

void	request::send_body_from_str() {
	int amount_sent;
	int to_send;
	for (int i = 10; i != 0; i--) {
		to_send = SEND_SPEED < body_response.size() ? SEND_SPEED : body_response.size();
		amount_sent = send(csock, body_response.c_str(), to_send, 0);
		std::cout << " We are sending from string : " << body_response.c_str() << std::endl;
		if (amount_sent == -1) {
			g_logger.fd << g_logger.get_timestamp() << "Issue while sending body on csock " << csock << ". Error: " << strerror(errno) << std::endl;
			body_is_sent = true;
			close_csock = true;
			return ;
		}
		else if (amount_sent != SEND_SPEED) {
			body_is_sent = true;
			break;
		}
		body_response = body_response.substr(to_send, body_response.size() - to_send);
	}
}

void	request::send_body_from_file() {
	int amount_read;
	int amount_sent;

	char buff[SEND_SPEED + 1];

	g_logger.fd << g_logger.get_timestamp() << "We are trying to send body from file: -" << conf->path_to_target.c_str() << std::endl;

	if (body_fd == -1) {
		body_fd = open(conf->path_to_target.c_str(), O_RDONLY);
		if (body_fd == -1) {
			g_logger.fd << g_logger.get_timestamp() << "Issue while opening -" << conf->path_to_target.c_str() << "- file on csock " << csock << ". Error: " << strerror(errno) << std::endl; // end special cases ?
			body_is_sent = true; // a check ce qu'on fait plutot envoyer une erreur 500
			close_csock = true;
			return ;
		}
		g_logger.fd << g_logger.get_timestamp() << "We opened file -" << conf->path_to_target.c_str() << "- file on fd " << body_fd << std::endl; // end special cases ?
	}
	for (int i = 4; i != 0; i--) {
		g_logger.fd << g_logger.get_timestamp() << "We are reading : " << conf->path_to_target << std::endl;
		amount_read = read(body_fd, buff, SEND_SPEED);
		buff[amount_read] = '\0';
		amount_sent = send(csock, buff, amount_read, 0);
		std::cout << " We are sending from file: " << buff << std::endl;
		if (amount_read != SEND_SPEED) { // a voir avec ALEXXXXXXXXX-----------------------------------------------------------
			body_is_sent = true;
			g_logger.fd << g_logger.get_timestamp() << "We are done sending : " << conf->path_to_target << "to csock : " << csock << std::endl;
			break;
		}
	}
}

request::~request() {
	if (body_fd != -1)
		close(body_fd);
	if (conf && conf->cgi_activated == true && cgi) {
		erase_static_fd_from_request(cgi->pipefd[0]);
		if (method == "POST")
			erase_static_fd_from_request(cgi->pipefd_post[1]);
	}
	delete conf;
	conf = NULL;
	delete resp;
	conf = NULL;
	delete cgi;
	cgi = NULL;
}

void	request::erase_static_fd_from_request(int fd) {
	g_webserv.static_fds.erase(fd);
	epoll_ctl(g_webserv.get_epfd(), EPOLL_CTL_DEL, fd, NULL);
}

void request::initiate_CGI_GET() {

	int ret = pipe(cgi->pipefd);
	//fcntl(cgi->pipefd[0], F_SETFL, O_NONBLOCK);
	if (ret == -1) {
		g_logger.fd << g_logger.get_timestamp() << "The pipe didn't work\n";
		code_to_send = 500; // a voir/tester avec suite du programme
		conf->local_actions_done = true;
		close_csock = true;
		return ;
	}
	cgi->pid = fork();
	if (cgi->pid == -1) {
		g_logger.fd << g_logger.get_timestamp() << "The fork didn't work\n";
		code_to_send = 500; // a voir/tester avec suite du programme
		conf->local_actions_done = true;
		close_csock = true;		
		close(cgi->pipefd[0]);
		close(cgi->pipefd[1]);
		return ;
	}
	if (cgi->pid == 0) //(fils)
	{
		g_logger.fd << g_logger.get_timestamp() << "I am child and I am about to execve CGI\n";
		close(cgi->pipefd[0]);
		dup2(cgi->pipefd[1], 1);
		close(cgi->pipefd[1]);
		char *arg2[] = {(char *)conf->_cgi_dir.c_str(), (char *)conf->path_to_target.c_str(), NULL};
		g_logger.fd << g_logger.get_timestamp() << conf->_cgi_dir << std::endl;
		g_logger.fd << g_logger.get_timestamp() << conf->path_to_target << std::endl;
		for (int i = 0; i < 14; i++) {
			g_logger.fd << g_logger.get_timestamp() << cgi->getenv()[i] << std::endl;
		}
		ret = execve(conf->_cgi_dir.c_str(), arg2, cgi->getenv());
		g_logger.fd << g_logger.get_timestamp() << "I am child and execve returned :" << ret << std::endl;
		if (ret == -1)
			exit(1);
		g_logger.fd << g_logger.get_timestamp() << "I am child and THIS SHOULD NOT HAPPEN\n";
	}
	if (cgi->pid != 0) //(daron)
	{
		g_logger.fd << g_logger.get_timestamp() << "I am father\n";
		close(cgi->pipefd[1]);
		
		waitpid(cgi->pid, &cgi->pid_status, WNOHANG);
		if (WIFEXITED(cgi->pid_status) == true && WEXITSTATUS(cgi->pid_status) != 0) {
			code_to_send = 500; // a voir/tester avec suite du programme
			conf->local_actions_done = true;
			close_csock = true;
			close(cgi->pipefd[0]);
			g_logger.fd << g_logger.get_timestamp() << "I am father and child " << cgi->pid << " exited with " << cgi->pid_status << std::endl;
			return ;
		}
		add_fd_epollin_to_pool(cgi->pipefd[0]);
		conf->local_actions_done = true;
		cgi->setCgi_stage("READFROM");
		return ;
	}
}

void request::initiate_CGI_POST() {
	g_logger.fd << g_logger.get_timestamp() << "We are about to write body_request inside the pipe for cgi" << std::endl;

	int ret = pipe(cgi->pipefd);
	fcntl(cgi->pipefd[0], F_SETFL, O_NONBLOCK);
	if (ret == -1) {
		g_logger.fd << g_logger.get_timestamp() << "The 1ST pipe for POST didn't work\n";
		code_to_send = 500; // a voir/tester avec suite du programme
		conf->local_actions_done = true;
		close_csock = true;
		return ;
	}		
	ret = pipe(cgi->pipefd_post);
	if (ret == -1) {
		g_logger.fd << g_logger.get_timestamp() << "The 2nd pipe for POST didn't work\n";
		code_to_send = 500; // a voir/tester avec suite du programme
		conf->local_actions_done = true;
		close_csock = true;
		close(cgi->pipefd[0]);
		close(cgi->pipefd[1]);
		return ;
	}
	cgi->pid = fork();
	if (cgi->pid == -1) {
		g_logger.fd << g_logger.get_timestamp() << "The fork for POST didn't work\n";
		code_to_send = 500; // a voir/tester avec suite du programme
		conf->local_actions_done = true;
		close_csock = true;
		close(cgi->pipefd[0]);
		close(cgi->pipefd[1]);
		close(cgi->pipefd_post[0]);
		close(cgi->pipefd_post[1]);
		return ;
	}
	if (cgi->pid == 0) //(fils)
	{
		close(cgi->pipefd[0]);
		dup2(cgi->pipefd[1], 1);
		close(cgi->pipefd[1]);
		close(cgi->pipefd_post[1]);
		dup2(cgi->pipefd_post[0], 0);
		close(cgi->pipefd_post[0]);
		char *arg2[] = {(char *)conf->_cgi_dir.c_str(), (char *)conf->path_to_target.c_str(), NULL};
		ret = execve(conf->_cgi_dir.c_str(), arg2, cgi->getenv());
		if (ret == -1)
			exit(1);
	}
	if (cgi->pid != 0) //(daron)
	{
		close(cgi->pipefd[1]);
		close(cgi->pipefd_post[0]);
		waitpid(cgi->pid, &cgi->pid_status, WNOHANG);
		if (WIFEXITED(cgi->pid_status) == true && WEXITSTATUS(cgi->pid_status) != 0) {
			code_to_send = 500; // a voir/tester avec suite du programme
			conf->local_actions_done = true;
			close_csock = true;
			close(cgi->pipefd[0]);
			close(cgi->pipefd_post[1]);
			return ;
		}
		add_fd_epollin_to_pool(cgi->pipefd[0]);
		g_webserv.static_fds.insert(cgi->pipefd_post[1]); /////////////////////////////ici
		add_fd_epollout_to_pool(cgi->pipefd_post[1]);
		g_webserv.static_fds.insert(cgi->pipefd[0]);
		return ;
	}
}

void request::initiate_CGI() {
	g_logger.fd << g_logger.get_timestamp() << "We are about to handle a CGI request\n";
	if (cgi == NULL) {
		cgi = new CGI();
		cgi->setCgi_stage("WRITEIN");
		cgi->param_CONTENT_LENGTH(*this);
		cgi->param_CONTENT_TYPE(*this);
		cgi->param_GATEWAY_INTERFACE();
		cgi->param_PATH_INFO(*this);
		cgi->param_PATH_TRANSLATED(*this);
		cgi->param_QUERY_STRING(*this);
		cgi->param_REMOTE_ADDR();
		cgi->param_REQUEST_METHOD(*this);
		cgi->param_SCRIPT_NAME(*this);
		cgi->param_SERVER_NAME();
		cgi->param_SERVER_PORT();
		cgi->param_SERVER_PROTOCOL();
		cgi->param_SERVER_SOFTWARE();
		if (method == "GET")
			initiate_CGI_GET();
		if (method == "POST")
			initiate_CGI_POST();
		g_logger.fd << g_logger.get_timestamp() << "We are done initiating the CGI\n";

	}
	else {
		g_logger.fd << g_logger.get_timestamp() << "I am father and I am waiting for my child\n";

		waitpid(cgi->pid, &cgi->pid_status, WNOHANG);
		if (WIFEXITED(cgi->pid_status) == true && WEXITSTATUS(cgi->pid_status) != 0) {
			g_logger.fd << g_logger.get_timestamp() << "The child exited with an error\n";
			code_to_send = 500; // a voir/tester avec suite du programme
			close_csock = true;
			conf->local_actions_done = true;
			close(cgi->pipefd[0]);
			close(cgi->pipefd_post[1]);
			return ;
		}
		std::string towrite = body_request.substr(body_written_cgi, SEND_SPEED);
		if (can_I_write_in_fd(cgi->pipefd_post[1]) == false)
			return ;
		int ret = write(cgi->pipefd_post[1], towrite.c_str(), towrite.size());
		body_written_cgi += ret;
		if (body_written_cgi == body_request.size()) {
			conf->local_actions_done = true;
			close(cgi->pipefd_post[1]);
			cgi->setCgi_stage("READFROM");
		}
	}
	return ;
}

void request::readfrom_CGI() {
	g_logger.fd << g_logger.get_timestamp() << "I am about to read from CGI\n";

	char buf[SEND_SPEED + 1];
	int ret;
	waitpid(cgi->pid, &cgi->pid_status, WNOHANG);
	if (WIFEXITED(cgi->pid_status) == true && WEXITSTATUS(cgi->pid_status) != 0) {
		g_logger.fd << g_logger.get_timestamp() << "Child exited badly\n";
		if (cgi->started_answering_cgi == true)
			body_is_sent = true;
		code_to_send = 500;
		update_code_and_body();
		conf->local_actions_done = true;
		conf->cgi_activated = false;
		close_csock = true;
		close(cgi->pipefd[0]);
		return ;
	}
	else if (WIFEXITED(cgi->pid_status) && can_I_read_from_fd(cgi->pipefd[0]) == false){
		body_is_sent = true;
		return ;
	}
	else
		
	if (cgi->status_read == false) {
		read_first_line_cgi();
	}
	else {
		if (cgi->left_from_first_line.empty() == false) {
			ret = send(csock, cgi->left_from_first_line.c_str(), cgi->left_from_first_line.size(), 0);
			if (ret == -1) {
				close_csock = true;
				close(cgi->pipefd[0]);
				body_is_sent = true;
				return ;
			}
			cgi->left_from_first_line = cgi->left_from_first_line.substr(ret, cgi->left_from_first_line.size() - ret);
		}
		else {
			if (can_I_read_from_fd(cgi->pipefd[0]) == false) {
				g_logger.fd << g_logger.get_timestamp() << "there is nothing to read in pipe from cgi but I will try again\n";		
				return ;
			}
			ret = read(cgi->pipefd[0], buf, SEND_SPEED);
			if (ret == -1 && errno == EAGAIN){ // todel !!!!!!!!
				g_logger.fd << g_logger.get_timestamp() << "THIS SHOULD NOT HAPPEN : -1 after epoll told pipefd[0] is readable\n";		
				return ;
			}
			if (ret == -1) {
				g_logger.fd << g_logger.get_timestamp() << "We could not read form CGI\n";
				close_csock = true;
				close(cgi->pipefd[0]);
				body_is_sent = true;
				return ;
			}
			if (ret == 0) {
				body_is_sent = true;
				close(cgi->pipefd[0]);
				return ;
			}
			buf[ret] = '\0';
			ret = send(csock, buf, ret, 0);
			if (ret == -1) {
				g_logger.fd << g_logger.get_timestamp() << "We could not send in csock " << csock << std::endl;
				close_csock = true;
				close(cgi->pipefd[0]);
				body_is_sent = true;
				return ;
			}
		}
	}
	return ;
}

void request::read_first_line_cgi() {
	char buf[SEND_SPEED + 1];
	int ret;

	if (can_I_read_from_fd(cgi->pipefd[0]) == false) {
		g_logger.fd << g_logger.get_timestamp() << "there is nothing to read in pipe from cgi but I will try again\n";		
		return ;
	}
	ret = read(cgi->pipefd[0], buf, SEND_SPEED);
	if ((ret == -1 && errno != EAGAIN) || ret == 0) {
		close_csock = true;
		close(cgi->pipefd[0]);
		body_is_sent = true;
		return ;
	}
	if (ret == -1 && errno == EAGAIN) {
		g_logger.fd << g_logger.get_timestamp() << "there is nothing to read in pipe from cgi first_line but I will try again\n";
		return ;
	}
	buf[ret] = '\0';
	cgi->first_line += buf;
	size_t end_of_first_line = cgi->first_line.find("\r\n", 0);
	if (end_of_first_line == std::string::npos)
		return ;
	if (cgi->first_line.substr(0, 7) != "Status:")
		code_to_send = 200;
	else if (ft_string_is_digit(get_word(cgi->first_line, go_to_next_word(cgi->first_line, 0))))
		code_to_send = std::atoi(get_word(cgi->first_line, go_to_next_word(cgi->first_line, 0)).c_str());
	else
		code_to_send = 200;
	std::string status_line = response::get_status_line(*this);
	ret = send(csock, status_line.c_str(), status_line.size(), 0);
	if (ret == -1) {
		close_csock = true;
		close(cgi->pipefd[0]);
		body_is_sent = true;
		return ;
	}
	cgi->started_answering_cgi = true;
	cgi->status_read = true;
	end_of_first_line += std::strlen("\r\n");
	cgi->left_from_first_line = cgi->first_line.substr(end_of_first_line, cgi->first_line.size() - end_of_first_line); 
}


static int		unlink_or_rmdir(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
	std::string path(fpath);
	int status;

	if (is_directory(path))
		status = rmdir(fpath);
	else
		status = unlink(fpath);
	if (status != 0)
		g_logger.fd << g_logger.get_timestamp() << "An error occured while deleting -" << path << " errno: " << errno << " " << strerror(errno) << std::endl;
	return (status);
}
				
void	request::delete_directory(std::string & path, request & req) {
	std::cout << " We are about to delete " << path << std::endl;
	nftw(path.c_str(), unlink_or_rmdir, 30, FTW_DEPTH);
	req.code_to_send = 204;
	req.body_response = response::generate_error_body(g_webserv.status_code.find(req.code_to_send)->second);
	req.conf->local_actions_done = true;
}

void	request::delete_file(std::string & path, request & req) {
	std::cout << " We are about to delete " << path << std::endl;
	int status = unlink(path.c_str());
	
	req.code_to_send = 204;
	req.body_response = response::generate_error_body(g_webserv.status_code.find(req.code_to_send)->second);
	req.conf->local_actions_done = true;
}
