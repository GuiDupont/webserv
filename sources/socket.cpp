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
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(param) close(param)

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;


// int main(void)
// {
//     SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
// 	sockaddr_in sin;

// 	sin.sin_addr.s_addr = htonl(INADDR_ANY);
// 	sin.sin_family = AF_INET;
// 	sin.sin_port = htons(23);
	
// 	bind(sock, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
// 	std::cout  << "coucou\n";
// 	listen(sock, 5);
// 	std::cout  << "coucou\n";

// 	socklen_t taille = sizeof(sin);
// 	std::cout  << "coucou\n";

// 	int csock = accept(sock, reinterpret_cast<sockaddr*>(&sin), &taille);
// 	std::cout  << "coucou\n";

// 	closesocket(csock);
// 	return EXIT_SUCCESS;
// }




typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#define PORT 1028
 
int main(void)
{
    #if defined (WIN32)
        WSADATA WSAData;
        int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
    #else
        int erreur = 0;
    #endif
    
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
    if(!erreur)
    {
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

					char buffer[2000];
					bzero(buffer, 1999);
					int ret;
					while ((ret = recv(csock, &buffer, sizeof(buffer), 0)) == 0)
					 	std::cout << "null\n";
					buffer[1999] = '\0';
					for (int i =0; i != 1999; i++)
						std::cout << buffer[i];
					std::cout << std::endl;
					std::cout << strerror(errno) << std::endl;
					send(csock, "HTTP/1.1 400 Bad Request", 24, 0);
					closesocket(csock);
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
        
        #if defined (WIN32)
            WSACleanup();
        #endif
    }
    
    return EXIT_SUCCESS;
}

