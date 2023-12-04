
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    
    char buffer[BUFFER_SIZE];

    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("webserver (socket) ");
    }

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //client address
    struct sockaddr_in client_addr;
    int client_addrlen  = sizeof(client_addr);


    /*

        Type casting sockaddr_in to sockaddr

    */

    if(bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) !=0){
        perror("webserver (bind)");
        return 1;
    }

    printf("socket created successfully \n");

    //Listening 
    if( listen(sockfd, SOMAXCONN) != 0 ) {
        perror("webserver (listen)");
        return 1;
    }

    //accept
    printf("server listening for connections\n");
    for(;;){
        //accept a socket connection
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if(newsockfd < 0 ) {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        //getting client address
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if(sockn < 0){
            perror("webserver (getsocketname)");
            continue;
        }

        //reading from the returned file descriptor
        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if(valread < 0){
            perror("webserver (read)");
            continue;
        }

        // Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);

        //write to socket
        int valwrite = write(newsockfd, resp, strlen(resp));
        if( valwrite < 0 ){
            perror("webserver (write)");
            continue;
        }
        close(newsockfd);
    }

    return 0;
}

