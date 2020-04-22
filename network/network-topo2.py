#Custom experiment topo

from mininet.topo import Topo

class MyTopo(Topo):
    def __init__(self):
        Topo.__init__(self)

        h1 = self.addHost('h1',ip='192.168.0.1/24',defaultRoute='via 192.168.0.100')
        h2 = self.addHost('h2',ip='192.168.0.2/24',defaultRoute='via 192.168.0.100')
        h3 = self.addHost('h3',ip='192.168.1.1/24',defaultRoute='via 192.168.1.100')
        h4 = self.addHost('h4',ip='192.168.2.1/24',defaultRoute='via 192.168.2.100')
        
        s1 = self.addSwitch('s1')
        r1 = self.addNode('r1',ip='192.168.0.100/24') 
        s3 = self.addSwitch('s3')
        self.addLink(h1,s1)
        self.addLink(h2,s1)
        self.addLink(s1,r1,intfName2='r1-eth0',params2={'ip':'192.168.0.100/24'})
        self.addLink(h3,r1,intfName2='r1-eth1',params2={'ip':'192.168.1.100/24'})
        self.addLink(s3,r1,intfName2='r1-eth2',params2={'ip':'192.168.2.100/24'})
        self.addLink(s3,h4)
        #self.start()   
        #switch.cmd('ovs-vsctl set-manager tcp:6640')

topos = {'mytopo':(lambda:MyTopo())}




