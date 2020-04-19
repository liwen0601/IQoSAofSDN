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

TCP=6
UDP=17

ip_protocol=TCP



dst_port=5001





class SimpleSwitch13(app_manager.RyuApp):
    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(SimpleSwitch13, self).__init__(*args, **kwargs)
        self.mac_to_port = {}
        self.queue_list = {}
        self.ovsdb_addr = "tcp:127.0.0.1:6640"
        #self.ovs_bridge = None
        self.ovs_vsctl = vsctl.VSCtl(self.ovsdb_addr)

    @set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
    def switch_features_handler(self, ev):
        datapath = ev.msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser
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

        queue_config=[]
        config={}
        config['max-rate']="500000000"
        config['min-rate']="0"
        queue_config.append(config)
        config={}
        config['max-rate']="5000000"
        config['min-rate']="0"
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
            actions=[parser.OFPActionSetQueue(1),parser.OFPActionOutput(out_port)]
            # verify if we have a valid buffer_id, if yes avoid to send both
            # flow_mod & packet_out
            if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                self.add_flow(datapath, 1, match, actions, msg.buffer_id)
            else:
                self.add_flow(datapath, 1, match, actions)
            #match = parser.OFPMatch(ip_proto="0x06",in_port=in_port, eth_dst=dst, eth_src=src)
            if ip_protocol == 6:
                match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=ip_protocol,tcp_dst=dst_port)
            else:
                match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=ip_protocol,udp_dst=dst_port)
            actions=[parser.OFPActionSetQueue(0),parser.OFPActionOutput(out_port)]
            # verify if we have a valid buffer_id, if yes avoid to send both
            # flow_mod & packet_out
            #print("add TCP")
            if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                self.add_flow(datapath, 2, match, actions, msg.buffer_id)
            else:
                self.add_flow(datapath, 2, match, actions)


            if ip_protocol == 6:
                match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=ip_protocol,tcp_src=dst_port)
            else:
                match = parser.OFPMatch(in_port=in_port, eth_dst=dst, eth_src=src,eth_type=0x0800,ip_proto=ip_protocol,udp_src=dst_port)
            actions=[parser.OFPActionSetQueue(0),parser.OFPActionOutput(out_port)]
            # verify if we have a valid buffer_id, if yes avoid to send both
            # flow_mod & packet_out
            #print("add TCP")
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
