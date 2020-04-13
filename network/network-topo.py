#Custom experiment topo
'''
from mininet.topo import Topo

class MyTopo(Topo):
    def __init__(self):
        Topo.__init__(self)

        host1 = self.addHost('h1')
        host2 = self.addHost('h2')

        switch = self.addSwitch('s1')

        self.addLink(host1,switch)
        self.addLink(host2,switch)

        #self.start()   
        #switch.cmd('ovs-vsctl set-manager tcp:6640')

topos = {'mytopo':(lambda:MyTopo())}
'''



from mininet.net import Mininet
from mininet.node import Controller, RemoteController
from mininet.cli import CLI
from mininet.log import setLogLevel, info

def emptyNet():
    net = Mininet( controller=RemoteController )
    net.addController( 'c0' )
    h1 = net.addHost( 'h1' )
    h2 = net.addHost( 'h2' )
    s1 = net.addSwitch( 's1' )
    s2 = net.addSwitch( 's2' )
    net.addLink( h1, s1 )
    net.addLink( h2, s1 )

    net.start()
    #s1.cmd('ovs-vsctl set-controller s1 remote:127.0.0.1:6654')
    #s1.cmd('ovs-vsctl set-manager ptcp:6640')
    
    CLI( net )
    net.stop()
    
if __name__ == '__main__':
    #setLogLevel( 'info' )
    emptyNet()

