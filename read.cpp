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

ssize_t writefile(int sockfd, FILE *fp);

// $ read {filename} {user} {group}
int main(int argc, char* argv[]){
    if (argc != 4){
        perror("usage:read <filename> <user> <group>");
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


    // configure the filename
    char *filename = basename(argv[1]);
    char buff[BUFFSIZE] = {0};
    // send the `filename` to server
    strncpy(buff,filename,strlen(filename));
    if (send(sockfd,buff,BUFFSIZE,0) == -1){
        perror("can't send filename");
        exit(1);
    }

    // configure the `action`
    char actbuff[BUFFSIZE] = {"read"};
    // send the `action` to server
    if (send(sockfd, actbuff, BUFFSIZE,0) == -1){
        perror("can't send action");
        exit(1);
    }

    // send its user name
    char user[BUFFSIZE] = {0};
    strncpy(user,argv[2],strlen(argv[2]));
    if (send(sockfd, user, BUFFSIZE, 0) == -1){
        perror("can't send user");
        exit(1);
    }

    // send its group name
    char group[BUFFSIZE] = {0};
    strncpy(group,argv[3],strlen(argv[3]));
    if (send(sockfd, group, BUFFSIZE, 0) == -1){
        perror("can't send group");
        exit(1);
    }

    // recv its msg, is about read privilage
    char msg[BUFFSIZE] = {0};
    recv(sockfd, msg, BUFFSIZE, 0);
    if (strcmp(msg,"fail") == 0){
        perror("read fail : file is not exist or you have no permission\n");
        exit(1);
    }else if (strcmp(msg,"succuss") == 0){
        // do-nop
    }else{
        // synchronization fail
        perror("read fail : synchronization prob\n");
        exit(1);
    }

    // open a new file that ready to be write
    FILE *fp = fopen(argv[1],"w");
    ssize_t total_bytes = writefile(sockfd,fp);

    sleep(5);

    printf("read success, numbytes = %ld\n",total_bytes);
    fclose(fp);
    close(sockfd);
    return 0;
}

ssize_t writefile(int sockfd, FILE *fp){
    // create a new file on client
    ssize_t n;
    ssize_t total = 0;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0){
        total += n;
        if (n == -1){
            perror("Receive File From Server Error");
            exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n){
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE); // or bzeor(.)
    }
    return total;
}