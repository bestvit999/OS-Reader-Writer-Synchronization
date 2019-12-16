# change to client folder
cd client_folder

# read {filename} {permission} {user}
## capability will be update to `capabilityList.dat`
../chmod homework1.c rwxr-- alex # success
sleep 1
../chmod homework2.c rwxr-- john # success
sleep 1
../chmod homework3.c rwxr-x ken # success
sleep 1
../chmod homework4.c rwxrwx merry # success
sleep 1
../chmod homework5.c rwx--- alex # fail : no permission