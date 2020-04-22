To start the network topo and controller correctly, please configure the environment first:
1. Make sure your linux system has been installed python, pip and git.
2. Obtain Mininet:
  2.1 Get source code: git clone git://github.com/mininet/mininet
  2.2 Install: mininet/util/install.sh -a
3. Obtain ryu
  3.1 Get source code: git clone git://github.com/osrg/ryu.git
  3.2 Install relevant packages: pip install -r ryu/tools/pip-requires
  3.3 Install: python ryu/setup.py install

4. Copy "config_L4.ini" or "config_L5.ini" to "config.ini". You can modify the content based on the format. (Do not copy the comments)
5. In a independent console, 
   cd iqos/dpi
   make
   cd dpiDeamon
   add user ips to ip.properties  (Usually 10.0.0.1, 10.0.0.2, ...)
   sudo ./dpiDeamon   (Make sure run the dpiDeamon under the directory)
   You can enter ps to see the print information
   Type flow to see flows added to controller
6. Then start the Mininet with network topo and controller: 
   sudo ./start.sh
7. xterm h1 h2
   TCP bandwidth testing
   h1: ./server TCP 5001
   h2: ./client 10.0.0.1 5001 TCP youtube.pcapng
   UDP bandwidth testing (default server port 5001)
   h1: ./server UDP 5001
   h2: ./client 10.0.0.1 5001 UDP youtube.pcapng
   
   You can also add -p port_num to specify server port to test more cases.
   You can modify "config.ini" at running time. The new configuration would be enabled once you save the file.
   
8. Logs of controller can be found in controller.log



