#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr; // address of server we want to connect to
    struct hostent *server; // defines a host computer on the internet

    char buffer[256];

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error when opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr, "Error, host does not exist");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error when connecting");
    }

    FILE *file = fopen("index.html", "r");
    char html[1024];
    size_t html_len = fread(html, 1, sizeof(html) - 1, file);

    char http_header[512];
    sprintf(http_header, sizeof(http_header), "POST / HTTP/1.1\r\nContent-Length: %zu\r\nContent-Type: text/html\r\n\r\n", html_len);

    send(sockfd, http_header, strlen(http_header), 0);
    send(sockfd, html, html_len, 0);

    fclose(file);

    printf("Enter a message to send to server ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin); // reads incoming messages from stdin
    n = write(sockfd, buffer, strlen(buffer));

    if (n < 0) {
        error("Error when writing to socket");
    }

    printf("%s", buffer);

    return 0;
    
}