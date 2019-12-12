#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <semaphore.h> // synchronization for `read` & `write`
#include <pthread.h> // using thread to perform the `read` & `write`
#include "transfer.h"
#include "util.h"

ssize_t writefile(int sockfd, FILE *fp); // write the server file
ssize_t readfile(int sockfd, FILE *fp); // send file to client site
void changemod(int sockfd); // change mod
ssize_t create(int sockfd, FILE *fp); // create the file

// data set of synchronization
sem_t wrt = 1;
sem_t mux = 1;
int readcount = 0;

void * start(void * arg); // start the server site

int main(int argc, char * argv[]){
    // socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("can't allocate sockfd");
        exit(1);
    }
    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr)); // or bzero(.)
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT); // port


    // bind
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &serveraddr, sizeof(serveraddr));
    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) 
    {
        perror("Bind Error");
        exit(1);
    }


    // listen
    if (listen(sockfd, LINSTENPORT) == -1) 
    {
        perror("Listen Error");
        exit(1);
    }

    pthread_t tid[50];
    int t_no = 0;

    while(1){
        /*---- Accept call creates a new socket for the incoming connection ----*/
        // connection establishment
        socklen_t addrlen = sizeof(clientaddr);
        int connfd = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
        if (connfd == -1) 
        {
            perror("Connect Error");
            exit(1);
        }
        // for each client request creates a thread and assign the client request to it to process
        // so the main thread can entertain next request
        if( pthread_create(&tid[t_no++], NULL, &start, &connfd) != 0 )
            printf("Failed to create thread\n");

        if (t_no == 50)
            t_no = 0;
    }
    return 0;
}

void * start(void * arg){
    int connfd = *((int *)arg);

    // configure server file folder
    char path_to_file[BUFFSIZE] = {"server_folder/"};
    char filename[BUFFSIZE] = {0};


    // recv the filename
    if (recv(connfd, filename, BUFFSIZE, 0) == -1) 
    {
        perror("Can't receive filename");
        exit(1);
    }

    // configure path_to_file
    strcat(path_to_file,filename);
    
    // recv the action on file, e.g. create, read, write, changemod
    char act[BUFFSIZE] = {0};
    if (recv(connfd, act, BUFFSIZE, 0) == -1){
        perror("Can't receive action");
        exit(1);
    }


    // according the act[] to perform different operation
    if (strcmp(act, "create") == 0){
        //// critical section : perform `create` on server ////
        // recv its permission
        char permission[BUFFSIZE] = {0};
        if (recv(connfd, permission, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive permission");
            exit(1);
        }

        // recv its user name
        char user[BUFFSIZE] = {0};
        if (recv(connfd, user, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive user");
            exit(1);
        }

        // recv its group name
        char group[BUFFSIZE] = {0};
        if (recv(connfd, group, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive group");
            exit(1);
        }

        // fopen a new file on server
        FILE *fp = fopen(path_to_file,"w");
        if (fp == NULL){
            perror("can't open file");
            exit(1);
        }

        ssize_t total = create(connfd, fp);
        printf("create susccess, numbytes = %ld, permission is %s\n",total, permission);


        // update capability
        capability_list capList;
        readCapability(capList);
        capList.updateCapList(capability(permission,user,group,total,timeNow(),filename));

        capList.list();

        fclose(fp);
        //// end of critical section ////
    }

    if (strcmp(act, "read") == 0){
        //// critical section : perform `read`, i.e. send the file to client site ////

        // recv its user name
        char user[BUFFSIZE] = {0};
        if (recv(connfd, user, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive user");
            exit(1);
        }

        // recv its group name
        char group[BUFFSIZE] = {0};
        if (recv(connfd, group, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive group");
            exit(1);
        }

        // check capList
        capability_list capList;
        readCapability(capList);
        if(!capList.isReadPermit(user,group,filename)){
            send(connfd,"fail",BUFFSIZE,0);
            exit(1);
        }else{
            send(connfd,"succuss",BUFFSIZE,0);
        }

        // fopen the file that want to be `read(download)`
        FILE *fp = fopen(path_to_file,"r");
        ssize_t total = readfile(connfd, fp);

        printf("send file susccess, numbytes = %ld\n",total);

        fclose(fp);
        //// critical section ////
    }

    if (strcmp(act, "write") == 0){
        //// critical section : perform `write`, i.e. write the file from client to server site ////
        // where the `write mode` parameter can be either ‘o’ or ‘a’, 
        // which allows Ken to either overwrite the original file or append his data in the end of the file, respectively.

        // recv its user name
        char user[BUFFSIZE] = {0};
        if (recv(connfd, user, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive user");
            exit(1);
        }

        // recv its group name
        char group[BUFFSIZE] = {0};
        if (recv(connfd, group, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive group");
            exit(1);
        }

        // recv `write mode`
        char writemod[BUFFSIZE] = {0};
        if (recv(connfd, writemod, BUFFSIZE, 0) == -1){
            perror("`write`:Can't receive write mode");
            exit(1);
        }

        // check capList
        capability_list capList;
        readCapability(capList);
        if(!capList.isWritePermit(user,group,filename)){
            send(connfd,"fail",BUFFSIZE,0);
            exit(1);
        }else{
            send(connfd,"succuss",BUFFSIZE,0);
        }

        // fopen a exist file on server, if not exist then create a new file
        FILE *fp;
        if (strcmp(writemod,"o") == 0){
            // overwrite
            fp = fopen(path_to_file,"w");
        }else if (strcmp(writemod, "a") == 0){
            // append
            fp = fopen(path_to_file,"a");
        }else{
            // defualt
            fp = fopen(path_to_file,"w");
        }
        if (fp == NULL){
            perror("can't open file");
            exit(1);
        }

        ssize_t total = writefile(connfd, fp);
        printf("write file susccess, numbytes = %ld\n",total);


        // update capability list
        int index = capList.getIndexCapability(user,group,filename);
        capList.updateCapList(index,user,group,total,timeNow());
        
        fclose(fp);

        //// end of critical section ////
    }

    if (strcmp(act, "chmod") == 0){
        //// critical section : perform `change mode` ////

        // recv its chmod permission
        char permission[BUFFSIZE] = {0};
        if (recv(connfd, permission, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive chmod permission");
            exit(1);
        }

        // recv its user name
        char user[BUFFSIZE] = {0};
        if (recv(connfd, user, BUFFSIZE, 0) == -1){
            perror("`create`:Can't receive user");
            exit(1);
        }

        // check capList
        capability_list capList;
        readCapability(capList);
        if(!capList.isChmodPermit(user,filename)){
            send(connfd,"fail",BUFFSIZE,0);
            exit(1);
        }else{
            send(connfd,"succuss",BUFFSIZE,0);
        }
        capability cap = capList.getCapability(user,filename);
        cap.permission = permission;
        capList.updateCapList(cap);

        printf("change mode susccess, %s permission now is %s\n", filename, permission);
        
        //// end of critical section ////
    }

    // close(connfd);
    pthread_exit(NULL);
}

ssize_t create(int sockfd, FILE *fp){
    // create a new file on server, the file where is from client site
    ssize_t n;
    ssize_t total = 0;
    char buff[MAX_LINE] = {0};
    while((n = recv(sockfd, buff, MAX_LINE, 0)) > 0){
        total += n;
        if (n == -1){
            perror("Receive File from client Error");
            //exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n){
            perror("Write File Error");
            //exit(1);
        }
        memset(buff, 0, MAX_LINE); // or bzeor(.)
    }
    return total;
}

ssize_t readfile(int sockfd, FILE *fp){
    // read the file that client want to download
    ssize_t n;
    ssize_t total = 0;
    char sendline[MAX_LINE] = {0};
    while((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0){
        
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

ssize_t writefile(int sockfd, FILE *fp){
    // write a exist file on server, the file where is from client site
    ssize_t n;
    ssize_t total = 0;
    char buff[MAX_LINE] = {0};
    while((n = recv(sockfd, buff, MAX_LINE, 0)) > 0){
        total += n;
        if (n == -1){
            perror("Receive File from client Error");
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