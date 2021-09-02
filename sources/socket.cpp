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
    
    int sock_err;
    
    // std::fstream logfile;
	// logfile.open("logfile", std::ios::out);
 
        /* Création d'une socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Si la socket est valide */
	if (sock != INVALID_SOCKET)
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
			std::cout << "Listage du port " << PORT << std::endl;
			
			/* Si la socket fonctionne */
		if(sock_err != SOCKET_ERROR)
			{
				int epfd;
				epfd = epoll_create(1024);
				
				static struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLET ;
				ev.data.fd = sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
				
				/* Attente pendant laquelle le client se connecte */

				

				
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
						if ((events[i].events & EPOLLERR) ||
								(events[i].events & EPOLLHUP))
						{
								/* An error has occured on this fd, or the socket is not
									ready for reading (why were we notified then?) */
							fprintf (stderr, "epoll error\n");
							close(fd);
							return (1);
						}
						if (events[i].events & EPOLLIN)
						{
							std::cout << "flag epollin\n";
							std::cout << "Patientez pendant que le client se connecte sur le port " << PORT << std::endl;
							csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
							std::cout << "Un client se connecte avec la socket " << csock << " de " <<  inet_ntoa(csin.sin_addr) << " : " << htons(csin.sin_port);
							char buffer[2000];

							buffer[1999] = '\0';
							bzero(buffer, 1999);
							int ret = recv(fd, &buffer, sizeof(buffer), 0);
							std::cout << buffer << std::endl;
							std::cout << ret << std::endl;
						}
						if ((events[i].events & EPOLLOUT)) {
							std::cout << "flag epollout\n";

						}
							// return (1);
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
