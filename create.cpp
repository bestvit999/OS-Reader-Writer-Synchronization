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

std::vector<bool> parsePermission(std::string permission);
ssize_t readfile(int sockfd, FILE *fp);

// $ create {filename} {permission} {user} {group}
int main(int argc, char* argv[]){
    if (argc != 5){
        perror("usage:create <filename> <permission> <user> <group>");
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
    char actbuff[BUFFSIZE] = {"create"};
    // send the `action` to server
    if (send(sockfd, actbuff, BUFFSIZE,0) == -1){
        perror("can't send action");
        exit(1);
    }


    // send its permission
    char permission[BUFFSIZE] = {0};
    strncpy(permission,argv[2],strlen(argv[2]));
    if (send(sockfd, permission, BUFFSIZE, 0) == -1){
        perror("can't send permission");
        exit(1);
    }

    // send its user name
    char user[BUFFSIZE] = {0};
    strncpy(user,argv[3],strlen(argv[3]));
    if (send(sockfd, user, BUFFSIZE, 0) == -1){
        perror("can't send user");
        exit(1);
    }

    // send its group name
    char group[BUFFSIZE] = {0};
    strncpy(group,argv[4],strlen(argv[4]));
    if (send(sockfd, group, BUFFSIZE, 0) == -1){
        perror("can't send group");
        exit(1);
    }

    // open the file that want to be upload to server
    FILE *fp = fopen(argv[1],"r");
    ssize_t total_bytes = readfile(sockfd,fp);

    printf("send success, numbytes = %ld\n",total_bytes);
    fclose(fp);
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