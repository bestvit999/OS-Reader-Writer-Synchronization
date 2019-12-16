# change to client folder
cd client_folder

# write {filename} {user} {group} {overwrite/append}
## capability will be update to `capabilityList.dat`
../write homework1.c alex AOS o # success : own + overwrite
../write homework3.c ken AOS a # success : own + append
../write homework1.c john AOS o # fail : no permission
../write homework5.c merry CSE o # success : group member + overwrite