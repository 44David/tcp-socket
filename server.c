#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void receive_message(int);

// displays error when called and exits.
void error(const char *msg) {
    perror(msg); // produces a short error message on stderr 
    exit(1);
};

int main(int argc, char *argv[]) {
    // sockfd and newsockfd are file descriptors, 
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    int n, pid;
    char buffer[256];

    // a structure which contains the internet address, from netinet/in.h
    struct sockaddr_in serv_addr, cli_addr;

    // checks given args to see if user provided a port number
    if (argc < 2) {
        fprintf(stderr, "No port provided\n");
        exit(1);
    }
    
    // creates a new socket, 1st arg is the address domain, we use AF_INET to specify usage of Internet Domain
    // 2nd arg is the type of socket, we use SOCK_STREAM for a socket stream, where characters are read in a stream. 
    // 3rd arg defines what protocol to be used, 0 allows the OS to use most appropriate one.  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error when opening socket");
    }

    // bzero() sets all values in buffer to zero, takes in pointer to a buffer and the size of it.
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // atoi() converts string of digits to integer
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    // htons() converts port number to network byte order
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // bind() is used to bind a socket to address, it takes in our socket file descriptor, address and size of the address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error when binding.");
    }

    // allows listening on the socket for connections, 2nd arg is the backlog queue (number of connections that can be held waiting)
    listen(sockfd, 5);

    // accept() is used to block until a connection from client to server is present.
    clilen = sizeof(cli_addr);

    while (1) {

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            error("Error when accepting");
        }

        pid = fork();

        if (pid < 0) {
            error("Error on fork");
        }
        if (pid == 0) {
            close(sockfd);
            receive_message(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }

    }

    return 0;
}

void receive_message(int newsockfd) {
    int n;
    char buffer[256];

    // initialize a buffer using bzero(), then read from the socket. read() will block until something is read.
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    
    if (n < 0) {
        error("Error when reading from socket.");
    }

    printf("Message read from client: %s", buffer);

    n = write(newsockfd, "This message is written for the client.", 18);
    if (n < 0) {
        error("Error when writing to socket");
    }
}