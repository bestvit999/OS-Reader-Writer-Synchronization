# change to client folder
cd client_folder

# read {filename} {permission} {user}
## capability will be update to `capabilityList.dat`
../chmod homework1.c rwxr-- alex # success
sleep 1
../chmod homework2.c rwxr-- john # fail {no permission}
sleep 1
../chmod homework3.c rwxr-x ken # success {group member}
sleep 1
../chmod homework4.c rwxrwx merry # fail {no permission}