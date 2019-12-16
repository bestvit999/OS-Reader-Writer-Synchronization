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
#include <semaphore.h>
#include <pthread.h>
#include <vector>
#include <iostream>

ssize_t writefile(int sockfd, FILE *fp);

// reader
void * reader(void *arg);

#define N 5 // # of threads

int main(int argc, char * argv[]){
    // if (argc != 4){
    //     perror("usage:read <filename> <user> <group>");
    //     exit(1);
    // }
    char *argv1[] = {"read","homework1.c","alex","AOS"};
    char *argv2[] = {"read","homework2.c","john","AOS"};
    char *argv3[] = {"read","homework3.c","ken","AOS"};
    char *argv4[] = {"read","homework4.c","merry","CSE"};

    pthread_t my_thread[N];
    // pthread_create(&my_thread[1], NULL, &reader, (void *)argv1);
    pthread_create(&my_thread[2], NULL, &reader, (void *)argv2);
    pthread_create(&my_thread[3], NULL, &reader, (void *)argv3);
    // pthread_create(&my_thread[4], NULL, &reader, (void *)argv4);
    // pthread_exit(NULL);

    // read(argv1);
    // read(argv2);
}

// $ read {filename} {user} {group}
void * reader(void * arg){
    char **argv = (char**)arg;
    printf("%s %s %s\n",argv[1],argv[2],argv[3]);

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

    // recv its msg
    char msg[BUFFSIZE] = {0};
    recv(sockfd, msg, BUFFSIZE, 0);
    if (strcmp(msg,"fail") == 0){
        perror("read fail");
        exit(1);
    }

    // open a new file that ready to be write
    FILE *fp = fopen(argv[1],"w");
    ssize_t total_bytes = writefile(sockfd,fp);

    printf("read success, numbytes = %ld\n",total_bytes);

    fclose(fp);
    close(sockfd);
    pthread_exit(NULL);

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