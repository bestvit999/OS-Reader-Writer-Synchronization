#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"
#include <string>
#include <vector>

// $ chmod {filename} {permission} {user}
int main(int argc, char* argv[]){
    if (argc != 4){
        perror("usage:chmod <filename> <permission> <user>");
        exit(1);
    }

    // socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("can't allocate sockfd");
        exit(1);
    }
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);
    inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr);


    // connect
    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0){
        perror("connect error");
        exit(1);
    }


    // configure the filename, send the `filename` to server
    char *filename = basename(argv[1]);
    char buff[BUFFSIZE] = {0};
    strncpy(buff,filename,strlen(filename));
    if (send(sockfd,buff,BUFFSIZE,0) == -1){
        perror("can't send filename");
        exit(1);
    }

    // configure the `action`, send the `action` to server
    char actbuff[BUFFSIZE] = {"chmod"};
    if (send(sockfd, actbuff, BUFFSIZE,0) == -1){
        perror("can't send action");
        exit(1);
    }


    // send its chmod permission
    char permission[BUFFSIZE] = {0};
    strncpy(permission,argv[2],strlen(argv[2]));
    if (send(sockfd, permission, BUFFSIZE, 0) == -1){
        perror("can't send chmod permission");
        exit(1);
    }

    // send its user
    char user[BUFFSIZE] = {0};
    strncpy(user,argv[3],strlen(argv[3]));
    if (send(sockfd, user, BUFFSIZE, 0) == -1){
        perror("can't send user name");
        exit(1);
    }

    // recv the result
    char msg[BUFFSIZE] = {0};
    recv(sockfd, msg, BUFFSIZE, 0);
    if (strcmp(msg,"fail") == 0){
        printf("change mode fail, file is not exist or you are not the owner\n");
    }else{
        printf("change mode success, permission now is %s\n",permission);
    }

    close(sockfd);
    return 0;
}