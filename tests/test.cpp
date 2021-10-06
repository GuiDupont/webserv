#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <map>

class request {
    
    public:

		std::string 						method;
		std::string 						request_target;
		std::string 						HTTP_version;
		std::map<std::string, std::string>	header_fields;
		// std::list<std::string>				trailer;
		std::string							host_name;
		std::string							body;
		std::string							left;
		int									next_chunk; // -2 = trailer stage ; -1 = chunk stage
		int									nb_trailer_to_received;
		int									code_to_send;
		int									csock;
		int									stage; // 0 = new requete, header en cours ; 1 = body en cours ; 2 = requete complete
		// config								*conf;
		// response							*resp;
		bool								close_csock;
		bool								request_with_body;
		bool								body_is_sent;
		bool								header_is_sent;
		int									body_fd;
		friend std::ostream & operator<<(std::ostream & o,const request & r);
};

class CGI {

public:

	CGI() {

		for (int i = 0; i < 14; i++) {
			env[i] = 0;
		}
	}
	~CGI() {}

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
	char **getenv();

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
	char *env[14];

};	

static int size_dest(long n)
{
	char	*dest;
	int		count = 1;

	if (n < 0)
	{
		n = -n;
		count++;
	}
	while (n >= 10)
	{
		n = n / 10;
		count++;
	}
	return (count);
}

std::string ft_itos(int n)
{
	long	n_long;
	int		count;
	char	dest[20];

	n_long = n;
	count = size_dest(n);
	dest[count] = '\0';
	if (n_long < 0)
	{
		dest[0] = '-';
		n_long = -n_long;
	}
	count--;
	while (n_long > 9)
	{
		dest[count] = n_long % 10 + '0';
		n_long = n_long / 10;
		count--;
	}
	dest[count] = n_long % 10 + '0';
	return (std::string(dest));
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

	this->SERVER_PORT = "SERVER_PORT=0"; // a voir si possible d'avoir bon port
	this->env[10] = (char *)this->SERVER_PORT.c_str();
	return ;
}

void CGI::param_SERVER_NAME() {

	this->SERVER_NAME = "SERVER_NAME=42webserv";
	this->env[9] = (char *)this->SERVER_NAME.c_str();
	return ;
}

void CGI::param_SCRIPT_NAME(request &req) {

	// this->SCRIPT_NAME = "SCRIPT_NAME=" + req.conf.?????; // a voir avec Guillaume si paramétré
	this->env[8] = (char *)this->SCRIPT_NAME.c_str();
	return ;
}

void CGI::param_REQUEST_METHOD(request &req) {

	this->REQUEST_METHOD = "REQUEST_METHOD=" + req.method;
	this->env[7] = (char *)this->REQUEST_METHOD.c_str();
	return ;
}

void CGI::param_REMOTE_ADDR() {

	this->REMOTE_ADDR = "REMOTE_ADDR=0.0.0.0"; // a voir si possible d'avoir bonne IP
	this->env[6] = (char *)this->REMOTE_ADDR.c_str();
	return ;
}


void CGI::param_QUERY_STRING(request &req) {

	// this->QUERY_STRING = "QUERY_STRING=" + this->conf->query_string; // à voir avec Guillaume et test avec branch main
	this->env[5] = (char *)this->QUERY_STRING.c_str();
	return ;
}

void CGI::param_PATH_TRANSLATED(request &req) {

	// this->PATH_TRANSLATED = "PATH_TRANSLATED=" + this->conf->path_to_target; // à voir avec Guillaume et test avec branch main
	this->env[4] = (char *)this->PATH_TRANSLATED.c_str();
	return ;
}

void CGI::param_PATH_INFO(request &req) {

	//this->PATH_INFO = "PATH_INFO=" + this->conf->path_info; // à voir avec Guillaume et test avec branch main
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
		this->CONTENT_LENGTH = "CONTENT_LENGTH=" + ft_itos(req.body.size());
	this->env[0] = (char *)this->CONTENT_LENGTH.c_str();
	return ;
}

void	perform_CGI(request &req) {

	// if (new_CGI == 0) // flag à paramétrer et à rajouter dans req
	CGI cgi; // ajouter une variable CGI a requête + faire un new

	cgi.param_CONTENT_LENGTH(req);
	cgi.param_CONTENT_TYPE(req);
	cgi.param_GATEWAY_INTERFACE();
	cgi.param_PATH_INFO(req);
	cgi.param_PATH_TRANSLATED(req);
	cgi.param_QUERY_STRING(req);
	cgi.param_REMOTE_ADDR();
	cgi.param_REQUEST_METHOD(req);
	cgi.param_SCRIPT_NAME(req);
	cgi.param_SERVER_NAME();
	cgi.param_SERVER_PORT();
	cgi.param_SERVER_PROTOCOL();
	cgi.param_SERVER_SOFTWARE();
	// for (int i = 0; cgi.getenv()[i] != 0; i++) {
	// 	std::cout << cgi.getenv()[i] << std::endl;
	// }
	if (req.method == "GET") {
		// pipe (rajouter variable pipefd[2])
		if retour == -1 => a voir 
		// fork (rajouter variable pid_t)
		if retour == -1 => à voir
		if (var == 0) //(fils)
		{
			close(pipefd[0]);
			dup2(pipefd[1], 1);
			close(pipefd[1])
			execve => si -1, à voir
		}
		if (var != 0) //(père)
		{
			close(pipefd[1]);
			lire pipefd[0] puis send => directement ? on repasse par poll?
			rajouter le fait qu il faille changer le status en HTTP
		}
	}



	// if (new_CGI == 1)
}

int main(int argc, char **argv, char **env) {

	char arg1[] = "/usr/bin/php";
	// char arg1[] = "ubuntu_cgi_tester";
	std::string s1 = "/usr/bin/php";
	// std::string s1 = "ubuntu_cgi_tester";
	std::string s2 = "hello.php";
	char *arg2[] = {(char *)s1.c_str(), (char *)s2.c_str(), NULL};
	// char *arg2[] = {"/usr/bin/php", "hello.php", NULL};
	// execve(arg1, arg2, NULL);

	request req;
	std::string REQUEST = "/hello.php";

	// req.method = "GET";
	req.method = "POST";
	req.body = "12345678";
	req.header_fields.insert(std::pair<std::string, std::string>("Content-Type", "txt, jpg"));
	// std::string::iterator it = REQUEST.end();
	perform_CGI(req);
	return (0);
}