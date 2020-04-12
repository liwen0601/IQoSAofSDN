mn -c
ryu-manager controller.py 2> controller.log&
mn --custom ./network-topo.py --topo mytopo --controller=remote,ip=127.0.0.1,port=6653
killall ryu-manager
