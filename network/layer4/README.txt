To start the network topo and controller correctly, please configure the environment first:
1. Make sure your linux system has been installed python, pip and git.
2. Obtain Mininet:
  2.1 Get source code: git clone git://github.com/mininet/mininet
  2.2 Install: mininet/util/install.sh -a
3. Obtain ryu
  3.1 Get source code: git clone git://github.com/osrg/ryu.git
  3.2 Install relevant packages: pip install -r ryu/tools/pip-requires
  3.3 Install: python ryu/setup.py install
4. Then start the Mininet with network topo and controller: 
    sudo ./start.sh TCP/UDP to guarantee TCP or UDP
5. When first start Mininet, set openvswitch manager: 
    s1 ovs-vsctl set-manager "ptcp:6640"
   Then restart Mininet
6. TCP bandwidth testing
   xterm h1 h2
   h1: iperf -s
   h2: iperf -c 10.0.0.1 -b 100G
   UDP bandwidth testing
   h1: iperf -s -u
   h2: iperf -c 10.0.0.1 -b 100G -u
   We can see bandwidth of guaranteed protocol is limited at 500Mbps, while the other is limited at 5Mbps

7. Logs of controller can be found in controller.log



