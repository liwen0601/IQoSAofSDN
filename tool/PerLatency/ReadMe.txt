#########################################
# build the programs
#########################################
1. cd PerLatency && make clean && make
2. excutable files will be generated in the directory PerLatency/bin
	PerLatency# ll bin/
	-rwxr-xr-x 1 root  root  13224 Feb  4 15:06 client*
	-rwxr-xr-x 1 root  root  13120 Feb  4 15:06 server*

#########################################
# run the programs
#########################################
1. latency test
./server port mode=1
./client host port mode=1


2. performance test
./server port mode=0
./client host port mode=0

