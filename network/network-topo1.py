#Custom experiment topo

from mininet.topo import Topo

class MyTopo(Topo):
    def __init__(self):
        Topo.__init__(self)

        h1 = self.addHost('h1')
        h2 = self.addHost('h2')
        h3 = self.addHost('h3')
        h4 = self.addHost('h4')
        
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2') 
        s3 = self.addSwitch('s3')
        self.addLink(h1,s1)
        self.addLink(h2,s1)
        self.addLink(s2,s1)
        self.addLink(s2,h3)
        self.addLink(s2,s3)
        self.addLink(s3,h4)
        #self.start()   
        #switch.cmd('ovs-vsctl set-manager tcp:6640')

topos = {'mytopo':(lambda:MyTopo())}




