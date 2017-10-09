export DYLD_LIBRARY_PATH=/Users/eyildirim/Documents/xerces/lib/
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/Cellar/opencv-3.1/lib/

rm -f conf srv.lck
#run 2 DataSpaces Servers accepting 2 Clients
mpiexec -n 2 /Users/eyildirim/Documents/ds_install/bin/dataspaces_server -s 2 -c 2 &

sleep 5

#run 2 client parallel processes
#-c parameter in dataspaces_server parameters must match -n parameters in mpiexec
mpiexec -n 2 ./executable
