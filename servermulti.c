#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h> // Ajouté pour corriger 'errno undeclared'

#define PORT 4242
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    
    fd_set readfds;
    socklen_t addrlen; // Ajouté pour stocker la taille de l'adresse

    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listener on port %d \n", PORT);
    
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;
        
        for ( i = 0 ; i < max_clients ; i++) {
            sd = client_socket[i];
            
            if(sd > 0) {
                FD_SET(sd, &readfds);
            }
            
            if(sd > max_sd) {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }
        
        if (FD_ISSET(server_fd, &readfds)) {
            addrlen = sizeof(address); // Utilisation correcte de addrlen
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
        }
        
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            
            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    getpeername(sd, (struct sockaddr*)&address, &addrlen); // Utilisation correcte de addrlen
                    printf("Host disconnected, ip %s, port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("Client %d: %s\n", i, buffer);
                }
            }
        }
    }
    
    return 0;
}

