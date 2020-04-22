#!/usr/bin/python

"""
    This example create 3 sub-networks to connect 3 domain controllers.
    Each domain network contains 1 switches.
    Each switch connect to 2 hosts.
"""

from mininet.net import Mininet
from mininet.node import Controller, RemoteController, OVSSwitch
from mininet.cli import CLI
from mininet.log import setLogLevel, info
from mininet.link import Link, Intf, TCLink
from mininet.topo import Topo
import logging
import os


def multiControllerNet(con_num=3, sw_num=3, host_num=6):
    "Create a network from semi-scratch with multiple controllers."
    controller_list = []
    switch_list = []
    host_list = []

    net = Mininet(controller=None, switch=OVSSwitch, link=TCLink)

    for i in range(0, con_num):
        name = 'c%s' % str(i+1)
        c = RemoteController(name, ip='127.0.0.1', port=6653+i)
        net.addController(c)
        controller_list.append(c)
        print ("*** Creating %s" % name)

    print ("*** Creating switches")
    switch_list = [net.addSwitch('s%d' % n) for n in range(1, sw_num+1)]

    print ("*** Creating hosts")
    host_list = [net.addHost('h%d' % n) for n in range(1, host_num+1)]

    print ("*** Creating links of host2switch.")
    for i in range(0, sw_num):
        net.addLink(switch_list[i], host_list[i*2])
        net.addLink(switch_list[i], host_list[i*2+1])

    print ("*** Starting network")
    net.build()
    for c in controller_list:
        c.start()

    _No = 0
    SpC = int (sw_num/con_num)
    for i in range(0, sw_num, SpC):
        for j in range(0, SpC):
            switch_list[i+j].start([controller_list[_No]])
            _No += 1

    CLI(net)
    print ("*** Stopping network")
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    multiControllerNet(con_num=3, sw_num=3, host_num=6)