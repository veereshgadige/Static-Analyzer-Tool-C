#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <arpa/inet.h> 
#include <openssl/ssl.h> 
#include <openssl/err.h> 

int main(int argc, char *argv[]) { 
    int serverSocket, newSocket; 
    struct sockaddr_in serverAddr, clientAddr; 
    socklen_t addrSize; 
    char buffer[1024];
	char buffer1[1024];
    // Initialize OpenSSL
    SSL_library_init(); 
    SSL_CTX *sslContext;
   	sslContext = SSL_CTX_new(SSLv23_server_method()); 
    if (!sslContext) { 
        ERR_print_errors_fp(stderr); 
        exit(EXIT_FAILURE); 
    } 
    // Load server certificate and private key 
    SSL_CTX_use_certificate_file(sslContext, "server.crt", SSL_FILETYPE_PEM); 
    SSL_CTX_use_PrivateKey_file(sslContext, "server.key", SSL_FILETYPE_PEM); 
    // Create socket 
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
    if (serverSocket == -1) { 
        perror("Error creating socket"); 
        exit(EXIT_FAILURE); 
    } 
    // Setup server address 
    memset(&serverAddr, '\0', sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons(atoi(argv[1])); 
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    // Bind socket 
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) { 
        perror("Error binding"); 
        exit(EXIT_FAILURE); 
    } 
    // Listen 
    if (listen(serverSocket, 10) == -1) { 
        perror("Error listening"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Server listening on port 8080...\n"); 
    addrSize = sizeof(clientAddr); 
    newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize); 
    if (newSocket == -1) { 
        perror("Error accepting connection"); 
        exit(EXIT_FAILURE);
    } 
    printf("Connection established with client.\n"); 
    // Attach SSL to the socket 
    SSL *ssl = SSL_new(sslContext); 
    SSL_set_fd(ssl, newSocket);
    if (SSL_accept(ssl) != 1) { 
        ERR_print_errors_fp(stderr); 
        SSL_free(ssl); 
        close(newSocket); 
        close(serverSocket); 
        exit(EXIT_FAILURE); 
    } 
	SSL_read(ssl, buffer, sizeof(buffer));
	do {
		printf("Client msg :%s\n", buffer);
		printf("Enter server msg to send\n");
		memset(buffer, '\0', sizeof(buffer));
		memset(buffer1, '\0', sizeof(buffer));
		fgets(buffer1, sizeof(buffer), stdin);
		SSL_write(ssl, buffer1, sizeof(buffer));
		//SSL_listen(ssl, 1);
		SSL_read(ssl, buffer, sizeof(buffer));
		if (strncmp(buffer, "bye", 3) == 0) {
			printf("Exiting the program in server side\n");
			exit(EXIT_SUCCESS);
		}
		buffer1[0] = '\0';
	}while(1);
	/*
    while (1) { 
        // Receive data from client 
        SSL_read(ssl, buffer, sizeof(buffer)); 
		printf("size of the buffer :%s -> %d\n", buffer, strlen(buffer));
		if (strncmp(buffer, "bye", strlen(buffer) - 1) == 0) {
			printf("Exiting the server program\n");
			exit(EXIT_SUCCESS);
		}
        printf("Client msg : %s\n", buffer);
	   	memset(buffer, '\0', sizeof(buffer));
		printf("Enter the data to send to client");
		fgets(buffer, sizeof(buffer), stdin);
        SSL_write(ssl, buffer, strlen(buffer));
        memset(buffer, '\0', sizeof(buffer)); 
    } */
    SSL_shutdown(ssl); 
    SSL_free(ssl); 
    close(newSocket); 
    close(serverSocket); 
    SSL_CTX_free(sslContext); 
    return 0; 
} 
