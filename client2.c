#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char ** argv) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = "127.0.0.1";

    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        error("Address did not work");
    }

    char header[] = "GET /index.html HTTP/1.1\r\nHost:mozilla.org\r\n\r\n";

    int n = fwrite(sockfd, header, strlen(header));
    char buffer[2048];
    n = fread(sockfd, buffer, 2048);

    printf("%s", buffer);
    return 0;

}