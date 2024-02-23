#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 
#include <arpa/inet.h> 
#include <openssl/ssl.h> 
#include <openssl/err.h> 

int main(int argc, char *argv[]) { 
    int clientSocket; 
    struct sockaddr_in serverAddr; 
    char buffer[1024]; 
	char str2[1024];
    // Initialize OpenSSL 
    SSL_library_init(); 
    SSL_CTX *sslContext = SSL_CTX_new(SSLv23_client_method()); 
    if (!sslContext) { 
        ERR_print_errors_fp(stderr); 
        exit(EXIT_FAILURE); 
    } 
    // Create socket 
    clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
    if (clientSocket == -1) { 
        perror("Error creating socket"); 
        exit(EXIT_FAILURE); 
    } 
    // Setup server address 
    memset(&serverAddr, '\0', sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons(atoi(argv[1])); 
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    // Connect to server 
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) { 
        perror("Error connecting"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Connected to server.\n"); 
    // Attach SSL to the socket 
    SSL *ssl = SSL_new(sslContext); 
    SSL_set_fd(ssl, clientSocket); 
    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr); 
        SSL_free(ssl); 
        close(clientSocket); 
        SSL_CTX_free(sslContext); 
        exit(EXIT_FAILURE); 
    }
	SSL_write(ssl, buffer, sizeof(buffer));
	do {
		SSL_read(ssl, str2, sizeof(str2));
		printf("\nserver msg:%s",str2);
		printf("\nclient msg:");
		memset(buffer, '\0', sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		if (strncmp(str2,"bye", 3) == 0) {
			exit(EXIT_SUCCESS);
		}
		SSL_write(ssl, str2, sizeof(str2));
	}while(1);
   /*	
    while (1) { 
        printf("Client: "); 
        fgets(buffer, sizeof(buffer), stdin); 
        // Send data to server 
        SSL_write(ssl, buffer, strlen(buffer)); 
		if (strncmp(buffer, "bye", sizeof(buffer)) == 0) {
			printf("Exiting the client program\n");
			exit(EXIT_SUCCESS);
		}
        // Receive data from server 
        SSL_read(ssl, buffer, sizeof(buffer)); 
        printf("Server: %s\n", buffer); 
        memset(buffer, '\0', sizeof(buffer)); 
    }*/ 
    SSL_shutdown(ssl); 
    SSL_free(ssl); 
    close(clientSocket); 
    SSL_CTX_free(sslContext); 
    return 0; 
} 
