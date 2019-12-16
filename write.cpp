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
#include <iostream>

ssize_t readfile(int sockfd, FILE *fp);

// $ write {filename} {user} {group} {overwrite/append}
int main(int argc, char* argv[]){
    if (argc != 5){
        perror("usage:write <filename> <user> <group> <overwite/append>");
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
    char actbuff[BUFFSIZE] = {"write"};
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

    // configure the `write mode`, send the `write mode` to server
    char writemod[BUFFSIZE] = {0};
    strncpy(writemod,argv[4],strlen(argv[4]));
    if (send(sockfd, writemod, BUFFSIZE, 0) == -1){
        perror("can't send write mode");
        exit(1);
    }


    // check write permit, 
    char msg[BUFFSIZE] = {0};
    recv(sockfd, msg, BUFFSIZE,0);
    if (strcmp(msg,"fail") == 0){
        printf("write fail, file is not exist or you have no permission\n");
        close(sockfd);
        exit(1);
    }else if (strcmp(msg,"succuss") == 0){
        // do-nop
    }else{
        // synchronization fail
        printf("write fail : synchronization prob\n");
        exit(1);
    }


    // open the file that want to be upload to server
    FILE *fp = fopen(argv[1],"r");
    ssize_t total_bytes = readfile(sockfd,fp);

    sleep(5);

    if (!total_bytes){
        printf("write fail, numbytes = %ld\n",total_bytes);
        fclose(fp);
    }else{
        printf("write success, numbytes = %ld\n",total_bytes);
        fclose(fp);
    }

    close(sockfd);
    return 0;
}

ssize_t readfile(int sockfd, FILE *fp){  
    ssize_t n;
    ssize_t total = 0;
    char sendline[MAX_LINE] = {0};
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0){
        total += n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
    return total;
}