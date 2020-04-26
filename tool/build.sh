dirLatency=./PerLatency
cd $dirLatency
make clean && make
cd -

gcc client.c -o client -Wincompatible-pointer-types -lpcap 
gcc server.c -o server 