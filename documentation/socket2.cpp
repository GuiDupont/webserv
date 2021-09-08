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
void 	displayCsockFd(struct epoll_event * events, size_t size) {
	for (size_t i = 0; i < size; i++)
		std::cout << "Csock fd is : " << events[i].data.fd << std::endl;
}

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
	
	sin.sin_addr.s_addr = inet_addr("127.0.0.5");
	sin.sin_family = AF_INET;                 /* Protocole familial (IP) */
	sin.sin_port = htons(PORT);               /* Listage du port */
	int sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
	if(sock_err == SOCKET_ERROR)
		std::cout << "Pb avec ft bind\n" << errno << std::endl;
	sock_err = listen(sock, 5);
	if(sock_err == SOCKET_ERROR)
		std::cout << "Pb avec ft listen\n" << std::endl;
	int epfd = epoll_create(1);
	static struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
	struct epoll_event *events;
	events = (struct epoll_event *)calloc(64, sizeof(ev));
	char buffer[100] = "";
	while (1)
	{
		int nsfd = epoll_wait(epfd, events, 64, 0);
		// if (nsfd)
		// 	std::cout << nsfd << " évènements de capté(s)" << std::endl;
		if (nsfd == -1)
			std::cout << strerror(errno) << std::endl;
		for (int i = 0; i < nsfd; i++) {
			if (events[i].events & EPOLLIN && events[i].data.fd == sock) {
				csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
				std::cout << "On a accepté un client\n";
				//int ret = recv(csock, buffer, 99, 0);
				ev.events = EPOLLIN | EPOLLOUT;
				ev.data.fd = csock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);
				//std::cout << "recv: '" << buffer << "' - " << ret << std::endl;
				// sleep(10);
			}
			else if (events[i].events & EPOLLIN) {
				int ret = recv(events[i].data.fd, buffer, 99, 0);
				if (ret != 0) {
					std::cout << "EPOLLIN not for csock + read\n";
					std::cout << "recv: '" << buffer << "' - " << ret << " from " << events[i].data.fd<< std::endl;
					ev.events = EPOLLIN | EPOLLOUT;
					ev.data.fd = events[i].data.fd;
					epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &ev);
				}
			}
			if (events[i].events & EPOLLHUP) {
				std::cout << "EPOLLHUP\n";
				// events[i].events = 0;
				epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
			}
			if (events[i].events & EPOLLERR) {
				std::cout << "EPOLLHERR\n";
				// events[i].events = 0;

				epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
			}
			if (events[i].events & EPOLLOUT && events[i].data.fd != sock)
			{
				std::cout << "EPOLLOUT\n";
				ev.events = EPOLLIN;
				ev.data.fd = events[i].data.fd;
				epoll_ctl(epfd, EPOLL_CTL_MOD, ev.data.fd, &ev);
				std::cout << "send_serv = " << send(events[i].data.fd, "salut", 6, 0) << std::endl;
			}
		}
		bzero(buffer, 99);
		displayCsockFd(events, nsfd);
	}
	free(events);
	close(epfd);
	close(csock);
	close(sock);
}
