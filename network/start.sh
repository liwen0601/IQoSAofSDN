if [ $# -eq 1 ]; then
    ovs-vsctl set-manager ptcp:6640
    ovs-vsctl del-br s1
    ovs-vsctl -- --all destroy QoS -- --all destroy Queue
    sysctl -w net.ipv4.ip_forward=1
    #../iqos/dpi/dpiDeamon/dpiDeamon& 3> /dev/null
    mn -c
    if [ $1 -eq 3 ]; then
        ryu-manager controller.py --ofp-tcp-listen-port 6653 2> controller.log&
        ryu-manager controller.py --ofp-tcp-listen-port 6656 2> controller2.log&
    else
        ryu-manager controller.py --ofp-tcp-listen-port 6653 2> controller.log&
    fi
    python monitor.py&
    #ryu-manager simple_switch_13.py 2> controller.log&
    if [ $1 -eq 3 ]; then
        mn --custom ./network-topo"$1".py --topo mytopo --controller=remote,ip=127.0.0.1,port=6653 --controller=remote,ip=127.0.0.1,port=6656
    else
        mn --custom ./network-topo"$1".py --topo mytopo --controller=remote,ip=127.0.0.1,port=6653
    fi
    #mn --custom network-topo.py --controller=remote,ip=127.0.0.1,port=6653
    #python network-topo.py
    killall ryu-manager
    killall python
    killall dpiDeamon
else
    echo "sudo ./start.sh 1/2/3/4 to start respective topo"
fi
