#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h> // Inclure pour utiliser 'errno' et identifier les erreurs

#define PORT 4242 // Définir le port d'écoute du serveur
#define BUFFER_SIZE 1024 // Taille du buffer pour les messages entrants

int main() {
    int server_fd, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    
    fd_set readfds; // Ensemble de descripteurs de fichier pour select()
    socklen_t addrlen; // Variable pour stocker la taille des adresses

    // Initialiser tous les sockets clients à 0 pour ne pas les considérer comme connectés
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Créer un socket pour le serveur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Définir l'adresse du serveur
    address.sin_family = AF_INET; // Type d'adresse
    address.sin_addr.s_addr = INADDR_ANY; // Accepter toutes les adresses entrantes
    address.sin_port = htons(PORT); // Port d'écoute

    // Lier le socket à l'adresse et au port spécifiés
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Marquer le socket comme un socket d'écoute pour les connexions entrantes
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listener on port %d \n", PORT);
    
    // Boucle principale du serveur
    while(1) {
        // Réinitialiser l'ensemble de lecture
        FD_ZERO(&readfds);
        // Ajouter le socket serveur à l'ensemble
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;
        
        // Ajouter les sockets clients à l'ensemble
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if(sd > 0) {
                FD_SET(sd, &readfds);
            }
            if(sd > max_sd) {
                max_sd = sd;
            }
        }

        // Attendre une activité sur l'un des sockets (avec un timeout infini)
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        // Vérifier si l'activité est due à une nouvelle connexion
        if (FD_ISSET(server_fd, &readfds)) {
            addrlen = sizeof(address);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            // Informer de la nouvelle connexion
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
            // Ajouter le nouveau socket à l'ensemble des sockets clients
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }
        
        // Traiter les données entrantes de chaque client
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            
            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // Si le client est déconnecté
                    getpeername(sd, (struct sockaddr*)&address, &addrlen);
                    printf("Host disconnected, ip %s, port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    // Afficher le message reçu
                    buffer[valread] = '\0';
                    printf("Client %d: %s\n", i, buffer);
                }
            }
        }
    }
    
    return 0;
}

