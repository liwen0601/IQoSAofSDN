To start the network topo and controller correctly, please configure the environment first:
1. Make sure your linux system has been installed python, pip and git.
2. Obtain Mininet:
  2.1 Get source code: git clone git://github.com/mininet/mininet
  2.2 Install: mininet/util/install.sh -a
3. Obtain ryu
  3.1 Get source code: git clone git://github.com/osrg/ryu.git
  3.2 Install relevant packages: pip install -r ryu/tools/pip-requires
  3.3 Install: python ryu/setup.py install
4. Then start the network topo and controller: 
    sudo ./start.sh
5. Logs of controller can be found in controller.log



