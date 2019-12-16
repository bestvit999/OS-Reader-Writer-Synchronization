all : server create read write chmod
server : server.cpp util.cpp transfer.h util.h
	g++ -O2 server.cpp util.cpp -o server -lpthread
create : create.cpp transfer.h
	g++ -O2 create.cpp -o create -lpthread
read : read.cpp transfer.h
	g++ -O2 read.cpp -o read -lpthread
write : write.cpp transfer.h
	g++ -O2 write.cpp -o write -lpthread
chmod : chmod.cpp transfer.h
	g++ -O2 chmod.cpp -o chmod -lpthread
clrcap :
	> capabilityList.dat
clean :
	rm server create read write chmod server_folder/*