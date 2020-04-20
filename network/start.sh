ovs-vsctl set-manager ptcp:6640
ovs-vsctl del-br s1
ovs-vsctl -- --all destroy QoS -- --all destroy Queue
#../iqos/dpi/dpiDeamon/dpiDeamon& 3> /dev/null
mn -c
ryu-manager controller.py 2> controller.log&
python monitor.py&
#ryu-manager simple_switch_13.py 2> controller.log&
#mn --custom ./network-topo.py --topo mytopo --controller=remote,ip=127.0.0.1,port=6653
#mn --custom network-topo.py --controller=remote,ip=127.0.0.1,port=6653
python network-topo.py
killall ryu-manager
killall python
killall dpiDeamon
