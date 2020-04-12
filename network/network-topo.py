#Custom experiment topo

from mininet.topo import Topo

class MyTopo(Topo):
	def __init__(self):
		Topo.__init__(self)

		host1 = self.addHost('h1')
		host2 = self.addHost('h2')

		switch = self.addSwitch('s1')

		self.addLink(host1,switch)
		self.addLink(host2,switch)
topos = {'mytopo':(lambda:MyTopo())}


