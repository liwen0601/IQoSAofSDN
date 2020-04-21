#Custom experiment topo

from mininet.topo import Topo

class MyTopo(Topo):
    def __init__(self):
        Topo.__init__(self)

        h1 = self.addHost('h1',ip='192.168.0.1/24')
        h2 = self.addHost('h2',ip='192.168.0.2/24')
        h3 = self.addHost('h3',ip='192.168.1.1/24')
        h4 = self.addHost('h4',ip='192.168.1.2/24')
        
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        self.addLink(h1,s1)
        self.addLink(h2,s1)
        self.addLink(h3,s2)
        self.addLink(h4,s2)
        #self.start()   
        #switch.cmd('ovs-vsctl set-manager tcp:6640')

topos = {'mytopo':(lambda:MyTopo())}




