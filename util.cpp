#include <iostream>
#include <ctime>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include "util.h"

capability::capability(std::string permission, std::string user, std::string group, long int filesize, long int timestamp,std::string filename){
    this->permission = permission;
    this->user = user;
    this->group = group;
    this->filesize = filesize;
    this->timestamp = timestamp;
    this->filename = filename;
}

void capability::list(){
    printf("%s \t %s \t %s \t %ld \t %s \t %s\n",this->permission.c_str(),this->user.c_str(),this->group.c_str(),this->filesize,this->filename.c_str(),timeStamp(this->timestamp).c_str());

    // std::cout << this->permission << '\t'
    // << this->user << '\t'
    // << this->group << '\t'
    // << this->filesize << '\t'
    // << this->filename << '\t'
    // << timeStamp(this->timestamp); 
}

void capability_list::append(capability cap){
    this->capList.push_back(cap);
}

void capability_list::list(){
    for (int i = 0; i < this->capList.size();i++){
        this->capList[i].list();
    }
}

int capability_list::isReadPermit(std::string user, std::string group, std::string filename){
    for (int i = 0;i < this->capList.size();i++){
        // file owner
        if (this->capList[i].user == user &&
            this->capList[i].filename == filename)
        {
            char read = this->capList[i].permission[OWNERREAD];
            if ( read == 'r'){
                printf("%s is owner of %s\n",user.c_str(),filename.c_str());
                // std::cout << user << " is owner of " << filename << std::endl;
                return 1;
            }
        }
        // same group
        if (this->capList[i].group == group &&
            this->capList[i].filename == filename)
        {
            char read = this->capList[i].permission[GROUPREAD];
            if ( read == 'r'){
                printf("%s is group member of %s\n",user.c_str(),filename.c_str());
                // std::cout << user << " is group member of " << filename << std::endl;
                return 1;
            }
        }
    }
    printf("file is not exist, or you have no permission\n");
    // std::cout << "file is not exist, or you have no permission" << std::endl;
    return 0;
}

int capability_list::isWritePermit(std::string user, std::string group, std::string filename){
    for (int i = 0;i < this->capList.size();i++){
        // file owner
        if (this->capList[i].user == user &&
            this->capList[i].filename == filename)
        {
            char write = this->capList[i].permission[OWNERWRITE];
            if ( write == 'w'){
                printf("%s is owner of %s\n",user.c_str(),filename.c_str());
                // std::cout << user << " is owner of " << filename << std::endl;
                return 1;
            }
        }
        // same group
        if (this->capList[i].group == group &&
            this->capList[i].filename == filename)
        {
            char write = this->capList[i].permission[GROUPWRITE];
            if ( write == 'w'){
                printf("%s is group member of %s\n",user.c_str(),filename.c_str());
                // std::cout << user << " is group member of " << filename << std::endl;
                return 1;
            }
        }
    }
    printf("file is not exist, or you have no permission\n");
    // std::cout << "file is not exist, or you have no permission" << std::endl;
    return 0;
}

int capability_list::isChmodPermit(std::string user, std::string filename){
    for (int i = 0;i < this->capList.size();i++){
        // only file owner can `chagne mode`
        if (this->capList[i].user == user &&
            this->capList[i].filename == filename)
        {
            printf("%s is owner of %s\n", user.c_str(), filename.c_str());
            // std::cout << user << " is owner of " << filename << std::endl;
            return 1;
        }
    }
    printf("file is not exist, or you have no permission\n");
    // std::cout << "file is not exist, or you have no permission" << std::endl;
    return 0;
}

void capability_list::updateCapList(capability cap){
    // search exist capbility list
    // only `create` or `chmod` will to update capability list
    int isExist = 0;
    for (int i = 0;i < this->capList.size();i++){
        // a file is exist
        if (this->capList[i].filename == cap.filename &&
            this->capList[i].user == cap.user)
        {
            // update the capabiliy
            this->capList[i] = cap;
            isExist = 1;
            break;
        }
    }
    if (!isExist){
        this->capList.push_back(cap);
    }

    // write into the capabilityList.dat
    std::ofstream output("capabilityList.dat");
    for (int i = 0;i < this->capList.size();i++){
        output << capList[i].permission << ' ';
        output << capList[i].user << ' ';
        output << capList[i].group << ' ';
        output << capList[i].filesize << ' ';
        output << capList[i].timestamp << ' ';
        output << capList[i].filename << '\n';
    }
}

void capability_list::updateCapList(int index, std::string user,std::string group, long int filesize, long int timestamp){
    // only `create` or `chmod` or `write` will to update capability list
    
    this->capList[index].user = user;
    this->capList[index].group = group;
    this->capList[index].filesize = filesize;
    this->capList[index].timestamp = timestamp;

    // write into the capabilityList.dat
    std::ofstream output("capabilityList.dat");
    for (int i = 0;i < this->capList.size();i++){
        output << capList[i].permission << ' ';
        output << capList[i].user << ' ';
        output << capList[i].group << ' ';
        output << capList[i].filesize << ' ';
        output << capList[i].timestamp << ' ';
        output << capList[i].filename << '\n';
    }
}

capability capability_list::getCapability(std::string user, std::string filename){
    for (int i = 0;i < this->capList.size();i++){
        // a file is exist
        if (this->capList[i].filename == filename &&
            this->capList[i].user == user)
        {
            return this->capList[i];
        }
    }
    // return this->capList[index];
    perror("can't get capability");
    exit(1);
}

int capability_list::getIndexCapability(std::string user,std::string group, std::string filename){
    for (int i = 0;i < this->capList.size();i++){
        // a file is exist
        if (this->capList[i].filename == filename &&
            this->capList[i].user == user)
        {
            return i;
        }

        // same group
        if (this->capList[i].group == group &&
            this->capList[i].filename == filename)
        {
            return i;
        }
        
    }
    // return this->capList[index];
    perror("can't get index of capability");
    exit(1);
}

std::string timeStamp(long int now){
    return std::string(ctime(&now));
}

long int timeNow(){
    time_t t = std::time(0);
    long int now = static_cast<long int>(t);
    return now;
}

void readCapability(capability_list &capList){
    std::ifstream input("capabilityList.dat");
    std::string permission;
    std::string user;
    std::string group;
    std::string filesize;
    std::string time;
    std::string filename;
    while(input >> permission >> user >> group >> filesize >> time >> filename){
        capList.append(capability(permission,user,group,std::stol(filesize),std::stol(time),filename));
    }
}