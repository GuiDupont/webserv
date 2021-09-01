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
    
    int sock_err;
    
    std::fstream logfile;
	logfile.open("logfile", std::ios::out);
 
        /* Création d'une socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Si la socket est valide */
	if(sock != INVALID_SOCKET)
	{
		printf("La socket %d est maintenant ouverte en mode TCP/IP\n", sock);
		
		/* Configuration */
		sin.sin_addr.s_addr = htonl(INADDR_ANY);  /* Adresse IP automatique */
		sin.sin_family = AF_INET;                 /* Protocole familial (IP) */
		sin.sin_port = htons(PORT);               /* Listage du port */
		sock_err = bind(sock, (SOCKADDR*)&sin, recsize);
		
		/* Si la socket fonctionne */
		if(sock_err != SOCKET_ERROR)
		{
			/* Démarrage du listage (mode server) */
			sock_err = listen(sock, 5);
			logfile << "Listage du port " << PORT << std::endl;
			
			/* Si la socket fonctionne */
			if(sock_err != SOCKET_ERROR)
			{
				/* Attente pendant laquelle le client se connecte */
				logfile << "Patientez pendant que le client se connecte sur le port " << PORT << std::endl;
				csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
				logfile << "Un client se connecte avec la socket " << csock << " de " <<  inet_ntoa(csin.sin_addr) << " : " << htons(csin.sin_port);

				int epfd;
				epfd = epoll_create(1024);
				
				static struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLHUP | EPOLLOUT;
				ev.data.fd = csock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);
				struct epoll_event *events;
				events = (struct epoll_event *)calloc(64, sizeof(ev)); /*??? MAX EVENTS ??*/
				while (1) {
					// wait for something to do...
					std::cout << "ready to wait \n";
					int nfds = epoll_wait(epfd, events, 64, -1); /*timeout?*/
					std::cout << "j ai recu un event, nfds = " << nfds << "\n";
					// for each ready socket
					for(int i = 0; i < nfds; i++) {
						int fd = events[i].data.fd;
						if ((events[i].events & EPOLLIN))
							std::cout << "flag epollin\n";
						if (events[i].events & EPOLLOUT)
						{
							std::cout << "flag epollout\n";
							char buffer[2000];
							bzero(buffer, 1999);
							int ret = recv(fd, &buffer, sizeof(buffer), 0);
							std::cout << ret << std::endl;
							buffer[1999] = '\0';
							std::cout << buffer << std::endl;
							send(fd, "HTTP/1.1 400 Bad Request", 24, 0);
						}
						if (events[i].events & EPOLLHUP)
						{
							std::cout << "TIME TO CLOSE\n";
							close(fd);
							break;
						}
						
						// std::cout << strerror(errno) << std::endl;
						// close(fd);
					}

				}
				closesocket(csock);
				close(epfd);
			}
			else
				perror("listen");
		}
		else
			perror("bind");
		/* Fermeture de la socket client et de la socket serveur */
		printf("Fermeture de la socket client\n");
		printf("Fermeture de la socket serveur\n");
		closesocket(sock);
		printf("Fermeture du serveur terminée\n");
	}
	else
		perror("socket");
	return EXIT_SUCCESS;
}

