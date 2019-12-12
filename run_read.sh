# change to client folder
cd client_folder

# read {filename} {user} {group}
## capability will be update to `capabilityList.dat`
../read homework1.c alex AOS # success
sleep 1
../read homework1.c john AOS # fail {no permission}
sleep 1
../read homework2.c ken AOS # success {group member}
sleep 1
../read homework5.c alex AOS # fail {no permission}