# Socket Programming & Capability List & Synchronization in C
File transmission over TCP sockets in Linux system.
Multi-thread on server site to handle the service.
Capability List on server site to check the permission to the file.
Synchronization mechanism on server site, allow multiple reader, but only one writer.

## Compile

    make
    
## Usage

### server

    ./server

### client create

    ./run_create.sh

### client read
    cd client_folder # simulate the client site

    > execute the command on different shell

    ../read {filename} {user} {group}
    ../read homework1.c alex AOS # success
    ../read homework2.c alex AOS # success : group member
    ../read homework1.c ken Aos # fail : no permission

### client write

    cd client_folder # simulate the client site

    > execute the command on different shell

    ../write {filename} {user} {group} {o/a}
    ../write homework1.c alex AOS o # success
    ../write homework1.c alex AOS a # success
    ../write homework1.c john AOS o # fail : no permission

### change mode

    ./run_chmode.sh