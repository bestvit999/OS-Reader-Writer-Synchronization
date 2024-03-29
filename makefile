all : server create read write chmod
server : server.cpp util.cpp transfer.h util.h
	g++ -Wall -O3 server.cpp util.cpp -o server -lpthread
create : create.cpp transfer.h
	g++ -O2 create.cpp -o create
read : read.cpp transfer.h
	g++ -O2 read.cpp -o read
write : write.cpp transfer.h
	g++ -O2 write.cpp -o write
chmod : chmod.cpp transfer.h
	g++ -O2 chmod.cpp -o chmod
clrcap :
	> capabilityList.dat
clean :
	rm server create read write chmod server_folder/* \
	> capabilityList.dat