#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdlib.h>	   

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(param) close(param)


typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#define PORT 1028


int main(void)
{
    /* Socket et contexte d'adressage du serveur */
    SOCKADDR_IN sin;
    SOCKET sock;
    socklen_t recsize = sizeof(sin);
    
    /* Socket et contexte d'adressage du client */
    SOCKADDR_IN csin;
    SOCKET csock;
    socklen_t crecsize = sizeof(csin);
    
   
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		std::cout << "Pb avec ft sock\n" << std::endl;
	printf("La socket %d est maintenant ouverte en mode TCP/IP\n", sock);
		
	/* Configuration */
	sin.sin_addr.s_addr = htonl(INADDR_ANY);  /* Adresse IP automatique */
	sin.sin_family = AF_INET;                 /* Protocole familial (IP) */
	sin.sin_port = htons(PORT);               /* Listage du port */
	int sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
	if(sock_err == SOCKET_ERROR)
		std::cout << "Pb avec ft bind\n" << std::endl;
	sock_err = listen(sock, 5);
	if(sock_err == SOCKET_ERROR)
		std::cout << "Pb avec ft listen\n" << std::endl;
	int epfd = epoll_create(1);
	static struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
	struct epoll_event *events;
	events = (struct epoll_event *)calloc(64, sizeof(ev));
	while (1)
	{
		int nsfd = epoll_wait(epfd, events, 64, -1);
		std::cout << nsfd << " évènements de capté(s)" << std::endl;
		for (int i = 0; i < nsfd; i++) {
			if (events[i].events & EPOLLIN) {
				std::cout << "EPOLLIN\n";
				csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
				std::cout << "send_serv = " << send(csock, "salut", 6, 0) << std::endl;
			}
			if (events[i].events & EPOLLHUP)
				std::cout << "EPOLLHUP\n";
			if (events[i].events & EPOLLOUT)
				std::cout << "EPOLLOUT\n";
		}
	}
	free(events);
	close(epfd);
	close(csock);
	close(sock);
}
