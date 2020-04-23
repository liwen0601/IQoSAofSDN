if [ $# -eq 1 ]; then
    ovs-vsctl set-manager ptcp:6640
    ovs-vsctl del-br s1
    ovs-vsctl -- --all destroy QoS -- --all destroy Queue
    sysctl -w net.ipv4.ip_forward=1
    #../iqos/dpi/dpiDeamon/dpiDeamon& 3> /dev/null
    mn -c
	
	#start controller
    ryu-manager controller.py --ofp-tcp-listen-port 6653 2> controller.log&

	#monitor configure
    python monitor.py&
	
    #ryu-manager simple_switch_13.py 2> controller.log&
	
	#start network
    mn --custom ./network-topo"$1".py --topo mytopo --controller=remote,ip=127.0.0.1,port=6653

    #mn --custom network-topo.py --controller=remote,ip=127.0.0.1,port=6653
    #python network-topo.py
    killall ryu-manager
    killall python
    killall dpiDeamon
else
    echo "sudo ./start.sh 1/2 to start respective topo"
fi
