#include <vector>
#include <string>
#include <map>

#define OWNERREAD 0
#define OWNERWRITE 1
#define OWNEREXECUTE 2
#define GROUPREAD 3
#define GROUPWRITE 4
#define GROUPEXECUTE 5

#ifndef CAPLIST_PRIVILAGE_H
#define CAPLIST_PRIVILAGE_H
/* server will use this to do protection */
class capability;
class capability_list{
    public:
        std::vector<capability> capList;
    public:
        void append(capability);
        void list();
        int isReadPermit(std::string user, std::string group, std::string filename);
        int isWritePermit(std::string user, std::string group, std::string filename);
        int isChmodPermit(std::string user, std::string filename);        
        void updateCapList(capability);
        void updateCapList(int,std::string user,std::string group, long int filesize, long int timestamp);
        capability getCapability(std::string user, std::string filename);
        int getIndexCapability(std::string user,std::string group, std::string filename);


};
#endif

#ifndef CAPABILITY_PRIVILAGE_H
#define CAPABILITY_PRIVILAGE_H
/* server will use this to do protection */
class capability{
    public:
        std::string permission;
        std::string user;
        std::string group;
        long int filesize;
        long int timestamp;
        std::string filename;
    public:
        capability(std::string,std::string,std::string,long int,long int,std::string);
        void list();
};
#endif

#ifndef SYN_MUTEX_H
#define SYN_MUTEX_H

#define READING 1
#define WRITING 2

class synlock{
    public:
        std::map<std::string, int> map_syn;
    public:
        synlock();
        int isLock(std::string filename,int status); // determine the lock status of file
        void insert(std::string, int); // update the current lock status
        void remove(std::string); // remove the lock
};

#endif

std::string timeStamp(long int now);
long int timeNow();
void readCapability(capability_list &capList);