#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h> // Adresse inet

int main(int argc, char **argv) {

    // Init avec la def du socket, le message, la reponse serveur, et la definition du srveur
    
    int socket1;

    char* message = "test";

    char server_reply[2000];

    struct sockaddr_in server;

    // Def du socket

    socket1 = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPV4 | SOCK_STREAM = TCP

    if (socket1 == -1) {

        printf("Impossible de créer le socket");

    }

    return 0;


    // Def du port, à changer si serveur change, sachant que le premier c'est l'IP, deuxième l'IPV4 et le troisème le port

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(4242);

    if (connect(socket1, (struct sockaddr * )&server, sizeof(server)) < 0) {
        
        puts("Problème de connection");
        return 1;

    }

    puts("Connecté");
    return 0;

    // Envoie du message

    message = "GET / HTTP/1.1\r\n\r\n";
    if (send(socket1, message, strlen(message), 0) < 0) {

        puts("Erreur pendant l'envoie du mesage\n");
        return 1;

    }

    // Réponse du serveur ici

    if (recv(socket1, server_reply, 2000, 0) < 0) {

        puts("Erreur de la réponse serveur");

    }

    puts("Réponse reçu\n");
    puts(server_reply);

    puts("Message envoyé\n");

    close(socket1);
    return 0;
}
