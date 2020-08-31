// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 5580
#define MAXLINE 1024

// Driver code
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Need a remote IP bruh");
        return -1;
    }

    int sockfd;
    fd_set read_fds;

    struct sockaddr_in servaddr, target_addr, connected_client_addr;

    struct timeval tv; // Set select to timeout
    bzero(&tv, sizeof(tv));
    tv.tv_usec = 100000; // 100 milliseconds

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    memset(&target_addr, 0, sizeof(servaddr));

    target_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(target_addr.sin_addr));
    target_addr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        char buf[MAXLINE] = {0};

        FD_SET(sockfd, &read_fds);
        int n = select(sockfd + 1, &read_fds, NULL, 0, &tv);
        if (n < 0)
        {
            perror("ERROR Server : select()\n");
            close(sockfd);
            exit(1);
        }
        if (n != 0 && FD_ISSET(sockfd, &read_fds))
        {

            socklen_t len = sizeof(connected_client_addr);
            int nbytes = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&connected_client_addr, &len);
            if (nbytes < 0)
            {
                perror("ERROR in recvfrom()");
                close(sockfd);
                exit(1);
            }
            buf[nbytes] = '\0';

            printf("Read %d bytes, contents '%s'\n", nbytes, buf);

            char *msg = "Recieved!";
            sendto(sockfd, (const char *)msg, strlen(msg), 0, (struct sockaddr *)&connected_client_addr, sizeof(connected_client_addr));
        }

        sendto(sockfd, "a", 1, 0, (struct sockaddr *)&target_addr, sizeof(target_addr)); // Small thing to just shoot udp at a target until something fun happens
    }

    return 0;
}
