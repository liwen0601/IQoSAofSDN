# Copyright (C) 2011 Nippon Telegraph and Telephone Corporation.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from ryu.base import app_manager
from ryu.controller import ofp_event
from ryu.controller.handler import CONFIG_DISPATCHER, MAIN_DISPATCHER
from ryu.controller.handler import set_ev_cls
from ryu.ofproto import ofproto_v1_3
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types
from ryu.lib.packet import ipv4
from ryu.lib.ovs import bridge
from ryu.lib.ovs import vsctl
import time
import os
import threading

#TCP=6
#UDP=17

#ip_protocol=TCP

#dst_port=5001





class SimpleSwitch13(app_manager.RyuApp):
    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]
    lastModify = 0.0
    mode = 1 #mode: 1-Layer4; 2-Layer5; 3-Content
    L4 = [] #L4: 1-TCP; 2-UDP
    L5 = [] #L5: guaranteed L4 and port
    ev = 0

    def __init__(self, *args, **kwargs):
        super(SimpleSwitch13, self).__init__(*args, **kwargs)
        self.mac_to_port = {}
        self.queue_list = {}
        self.ovsdb_addr = "tcp:127.0.0.1:6640"
        #self.ovs_bridge = None
        self.ovs_vsctl = vsctl.VSCtl(self.ovsdb_addr)
        self.lastModify=os.path.getmtime("config.ini")
        print("init finish")


    def remove_table_flows(self, datapath, table_id, match, instructions):
        """Create OFP flow mod message to remove flows from table."""
        ofproto = datapath.ofproto
        flow_mod = datapath.ofproto_parser.OFPFlowMod(datapath, 0, 0,table_id,ofproto.OFPFC_DELETE,0, 0,1,ofproto.OFPCML_NO_BUFFER,ofproto.OFPP_ANY, ofproto.OFPG_ANY, 0,match, instructions)
        return flow_mod



    def reset(self):
        self.L4=[]
        self.L5=[]

        datapath = self.ev.msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser


        empty_match = parser.OFPMatch()
        instructions = []
        flow_mod = self.remove_table_flows(datapath, 0,empty_match, instructions)
        self.logger.info("deleting all flow entries in table")
        datapath.send_msg(flow_mod)
        
        #command = vsctl.VSCtlCommand("set",("port"))
        #self.ovs_vsctl.run_command([command])
        #print(command)
        self.ovs_bridge = bridge.OVSBridge(self.CONF,datapath.id,self.ovsdb_addr)
        try:
            self.ovs_bridge.init()
            self.logger.info("OVSBridge success")
        except:
            self.logger.info("OVSBridge fail")
            return
        
        vif_ports = self.ovs_bridge.get_port_name_list()
        
        f = open("config.ini","r")

        line = f.readline()
        self.mode = int(line)    #mode: 1-Layer4; 2-Layer5; 3-Content
        #self.logger.info(line)
        if self.mode == 1:
            self.logger.info("Layer 4 mode")
            '''
            line = f.readline()
            parameters = line.split(' ')
            self.L4.append(int(parameters[0]))
            maxRate = int(parameters[1])
            minRate = int(parameters[2])
            
            queue_config=[]
            config={}
            config['max-rate']=str(maxRate)
            config['min-rate']=str(minRate)
            queue_config.append(config)

            line = f.readline()
            parameters = line.split(' ')
            '''
            queue_config=[]
            while True:
                line = f.readline()
                parameters = line.split(' ')
                if len(parameters) != 3:
                    #print(parameters)
                    break
                self.L4.append(int(parameters[0]))
                maxRate = int(parameters[1])
                minRate = int(parameters[2])

                config={}
                config['max-rate']=str(maxRate)
                config['min-rate']=str(minRate)
                queue_config.append(config)

            maxRate = int(parameters[0])
            minRate = int(parameters[1])


            config={}
            config['max-rate']=str(maxRate)
            config['min-rate']=str(minRate)
            queue_config.append(config)

            for port_name in vif_ports:
                self.logger.info("Set QoS to port %s",port_name)
                try:
                    self.ovs_bridge.set_qos(port_name,type='linux-htb',queues=queue_config)
                    self.logger.info("QoS success")
                except:
                    self.logger.info("QoS fail")
        


        
        elif self.mode == 2:
            self.logger.info("Layer 5 mode")
            queue_config=[]
            while True:
                line = f.readline()
                parameters = line.split(' ')
                print(parameters)
                if len(parameters) != 4:
                    #print(parameters)
                    break
                self.L5.append((int(parameters[0]),int(parameters[1])))
                maxRate = int(parameters[2])
                minRate = int(parameters[3])

                config={}
                config['max-rate']=str(maxRate)
                config['min-rate']=str(minRate)
                queue_config.append(config)
                        
            maxRate = int(parameters[0])
            minRate = int(parameters[1])
    
            config={}
            config['max-rate']=str(maxRate)
            config['min-rate']=str(minRate)
            queue_config.append(config)           


            for port_name in vif_ports:
                self.logger.info("Set QoS to port %s",port_name)
                try:
                    self.ovs_bridge.set_qos(port_name,type='linux-htb',queues=queue_config)
                    self.logger.info("QoS success")
                except:
                    self.logger.info("QoS fail")
        
               
        # install table-miss flow entry
        #
        # We specify NO BUFFER to max_len of the output action due to
        # OVS bug. At this moment, if we specify a lesser number, e.g.,
        # 128, OVS will send Packet-In with invalid buffer_id and
        # truncated packet data. In that case, we cannot output packets
        # correctly.  The bug has been fixed in OVS v2.1.0.
        match = parser.OFPMatch()
        actions = [parser.OFPActionOutput(ofproto.OFPP_CONTROLLER,
                                          ofproto.OFPCML_NO_BUFFER)]
        self.add_flow(datapath, 0, match, actions)





    @set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
    def switch_features_handler(self,ev):
        self.ev=ev
        self.reset()
        self.lastModify = os.path.getmtime("config.ini")
        

    def add_flow(self, datapath, priority, match, actions, buffer_id=None):
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        inst = [parser.OFPInstructionActions(ofproto.OFPIT_APPLY_ACTIONS,
                                             actions)]
        if buffer_id:
            mod = parser.OFPFlowMod(datapath=datapath, buffer_id=buffer_id,
                                    priority=priority, match=match,
                                    instructions=inst)
        else:
            mod = parser.OFPFlowMod(datapath=datapath, priority=priority,
                                    match=match, instructions=inst)
        datapath.send_msg(mod)
        self.logger.info("Add a flow")

    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def _packet_in_handler(self, ev):
        # If you hit this you might want to increase
        # the "miss_send_length" of your switch
        #self.ovs_bridge.init()
        self.ev=ev

        f = 0
        lm = os.path.getmtime("config.ini")
        if lm > self.lastModify:
            self.logger.info("found modify, reset")
            self.lastModify = lm
            self.reset()

        



        if ev.msg.msg_len < ev.msg.total_len:
            self.logger.debug("packet truncated: only %s of %s bytes",
                              ev.msg.msg_len, ev.msg.total_len)
        msg = ev.msg
        datapath = msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser
        in_port = msg.match['in_port']
        ofp = datapath.ofproto
        pkt = packet.Packet(msg.data)
        eth = pkt.get_protocols(ethernet.ethernet)[0]

        ip = pkt.get_protocols(ipv4.ipv4)

        if eth.ethertype == ether_types.ETH_TYPE_LLDP:
            # ignore lldp packet
            return
        dst = eth.dst
        src = eth.src

        dpid = format(datapath.id, "d").zfill(16)
        self.mac_to_port.setdefault(dpid, {})

        self.logger.info("packet in %s %s %s %s %s", dpid, src, dst, in_port, ip)

        # learn a mac address to avoid FLOOD next time.
        self.mac_to_port[dpid][src] = in_port

        if dst in self.mac_to_port[dpid]:
            out_port = self.mac_to_port[dpid][dst]
        else:
            out_port = ofproto.OFPP_FLOOD

        #actions = [parser.OFPActionOutput(out_port)]

        #match = parser.OFPMatch(ip_proto=0x06)
        actions=[parser.OFPActionOutput(out_port),parser.OFPActionSetQueue(0)]


        # install a flow to avoid packet_in next time
        if out_port != ofproto.OFPP_FLOOD:
            match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src)
            if self.mode == 1:
                actions=[parser.OFPActionSetQueue(len(self.L4)),parser.OFPActionOutput(out_port)]
            elif self.mode == 2:
                actions=[parser.OFPActionSetQueue(len(self.L5)),parser.OFPActionOutput(out_port)]
            # verify if we have a valid buffer_id, if yes avoid to send both
            # flow_mod & packet_out
            if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                self.add_flow(datapath, 1, match, actions, msg.buffer_id)
            else:
                self.add_flow(datapath, 1, match, actions)
            #match = parser.OFPMatch(ip_proto="0x06",in_port=in_port, eth_dst=dst, eth_src=src)

            if self.mode == 1:   #L4 mode
                for i in range(len(self.L4)):
                    if self.L4[i] == 1:    #TCP
                        match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=6)
                    elif self.L4[i] == 2:  #UDP
                        match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=17)
                    actions=[parser.OFPActionSetQueue(i),parser.OFPActionOutput(out_port)]
            # verify if we have a valid buffer_id, if yes avoid to send both
            # flow_mod & packet_out
            #print("add TCP")
                    if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                        self.add_flow(datapath, 2, match, actions, msg.buffer_id)
                    else:
                        self.add_flow(datapath, 2, match, actions)

            elif self.mode ==2: #L5 mode
                for i in range(len(self.L5)):
                    if self.L5[i][0] == 1:
                        match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=6,tcp_src=self.L5[i][1]) 
                    elif self.L5[i][0] == 2:
                        match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=17,udp_src=self.L5[i][1])
                    actions=[parser.OFPActionSetQueue(i),parser.OFPActionOutput(out_port)]
                    if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                        self.add_flow(datapath, 2, match, actions, msg.buffer_id)
                    else:
                        self.add_flow(datapath, 2, match, actions)
                    
                    if self.L5[i][0] == 1:
                        match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=6,tcp_dst=self.L5[i][1]) 
                    elif self.L5[i][0] == 2:
                        match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=17,udp_dst=self.L5[i][1])
                    actions=[parser.OFPActionSetQueue(i),parser.OFPActionOutput(out_port)]
                    if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                        self.add_flow(datapath, 2, match, actions, msg.buffer_id)
                    else:
                        self.add_flow(datapath, 2, match, actions)


        data = None
        if msg.buffer_id == ofproto.OFP_NO_BUFFER:
            data = msg.data

        out = parser.OFPPacketOut(datapath=datapath, buffer_id=msg.buffer_id,
                                  in_port=in_port, actions=actions, data=data)
        datapath.send_msg(out)


