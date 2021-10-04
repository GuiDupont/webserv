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

	CGI() {}
	~CGI() {}

	void param_CONTENT_LENGTH(request &req);
	void param_CONTENT_TYPE(request &req);
	void param_GATEWAY_INTERFACE();
	void param_PATH_INFO();
	void param_PATH_TRANSLATED();
	void param_QUERY_STRING();
	void param_REMOTE_ADDR();
	void param_REQUEST_METHOD();
	void param_SCRIPT_NAME();
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
	std::string PATH_TRANSLATED; // obligatoire ? = transformation in server document
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

	CGI cgi;

	cgi.param_CONTENT_LENGTH(req);
	cgi.param_CONTENT_TYPE(req);
	for (int i = 0; cgi.getenv()[i] != 0; i++) {
		std::cout << cgi.getenv()[i] << std::endl;
	}
}

int main(int argc, char **argv, char **env) {

	char arg1[] = "/usr/bin/php";
	std::string s1 = "/usr/bin/php";
	std::string s2 = "hello.php";
	char *arg2[] = {(char *)s1.c_str(), (char *)s2.c_str(), NULL};
	// char *arg2[] = {"/usr/bin/php", "hello.php", NULL};
	// execve(arg1, arg2, NULL);

	request req;
	std::string REQUEST = "/hello.php";

	// req.method = "GET";
	req.method = "POST";
	req.body = "12345678";
	req.header_fields.insert(std::pair<std::string, std::string>("Content-Type", "txt"));
	// std::string::iterator it = REQUEST.end();
	perform_CGI(req);
	return (0);
}