#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // Adresse inet
#include <unistd.h>

int main(int argc, char **argv) {

    int socket1;
    char message[2000];
    char server_reply[2000];
    struct sockaddr_in server;
    int boucle = 1;

    // Création du socket
    socket1 = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP

    if (socket1 == -1) {
        printf("Impossible de créer le socket");
        return 1;
    }

    // Définition du port et connexion au serveur
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(4242);

    if (connect(socket1, (struct sockaddr *)&server, sizeof(server)) < 0) {
        puts("Problème de connexion");
        return 1;
    }

    puts("Connecté");
    
    while (boucle==1){

	    // Saisie utilisateur du message à envoyer
	    printf("Entrez votre message : ");
	    fgets(message, sizeof(message), stdin);

	    // Envoi du message
	    if (send(socket1, message, strlen(message), 0) < 0) {
		puts("Erreur pendant l'envoi du message\n");
		return 1;
	    }
	    usleep(100);

	    // Réception de la réponse du serveur
	    if (recv(socket1, server_reply, sizeof(server_reply), 0) < 0) {
		puts("Erreur de la réponse serveur");
		return 1;
	    }

	    puts("Réponse reçue\n");
	    puts(server_reply);
	    
    }

    // Fermeture du socket
    close(socket1);

    return 0;
}
