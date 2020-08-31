#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

#define PORT 5580
#define MAXLINE 1024

// Driver code
int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("%s <remote_ip> ", argv[0]);
    }

    int sockfd;
    fd_set read_fds;

    struct sockaddr_in servaddr, target_addr, connected_client_addr;

    struct timeval tv; // Set select to timeout
    bzero(&tv, sizeof(tv));

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

    FD_ZERO(&read_fds);

    int connected = 0;
    while (1)
    {

        FD_SET(sockfd, &read_fds);

        tv.tv_usec = (connected) ? 200000 : 50000; // If we're not connected send packets every 50ms, otherwise heartbeat every 200ms

        char buf[MAXLINE] = {0};
        printf("yes\n");

        int n = select(sockfd + 1, &read_fds, NULL, 0, &tv);
        if (n < 0)
        {
            perror("ERROR Server : select()\n");
            close(sockfd);
            exit(1);
        }

        if (FD_ISSET(sockfd, &read_fds))
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
            usleep(100000);
            connected = 1;
            continue;
        }

        if (!connected)
        {
            sendto(sockfd, "heartbeat", 10, 0, (struct sockaddr *)&target_addr, sizeof(target_addr)); // Small thing to just shoot udp at a target until something fun happens
        }
    }

    return 0;
}
