if [ "$#" -ne 2 ]; then
    echo "./start TCP/UDP DST_PORT to guarantee TCP/UDP DST_PORT"
else
    if [ "$1" == "TCP" ]; then
        sed -i "s/ip_protocol=UDP/ip_protocol=TCP/g" controller.py
    elif [ "$1" == "UDP" ]; then
        sed -i "s/ip_protocol=TCP/ip_protocol=UDP/g" controller.py
    else
        echo "./start TCP/UDP DST_PORT to guarantee TCP/UDP DST_PORT"
    fi
    sed -i "35c dst_port=$2" controller.py
    mn -c
    ryu-manager controller.py 2> controller.log&
#ryu-manager simple_switch_13.py 2> controller.log&
#mn --custom ./network-topo.py --topo mytopo --controller=remote,ip=127.0.0.1,port=6653
#mn --custom network-topo.py --controller=remote,ip=127.0.0.1,port=6653
    python network-topo.py
    killall ryu-manager
fi
