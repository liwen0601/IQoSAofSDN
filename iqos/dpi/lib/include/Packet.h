/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Packet parse
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _PACKET_H_
#define _PACKET_H_ 
#include <BasicType.h>

using namespace std;

enum ETHTYPE
{
    ETH_IPV4  = 0x0800,
    ETH_IPV6  = 0x86DD,
    ETH_ARP   = 0x0806,
};

class Packet
{
protected:
    DWORD m_PktLen;
    BYTE* m_PktData;

protected:
        
    virtual DWORD ParsePacket() = 0;
    
public:
    Packet (BYTE* PktData, DWORD PktLen)
    {
        m_PktData = new BYTE[PktLen];
        assert (m_PktData != NULL);
        memcpy (m_PktData, PktData, PktLen);

        m_PktLen  = PktLen;
    }

    ~Packet ()
    {
        if (m_PktData != NULL)
        {
            delete m_PktData;
            m_PktData = NULL;
        }
    }

    inline DWORD GetPacketData (BYTE** PktData)
    {
        *PktData = m_PktData;

        return m_PktLen;
    }

};

/*******************************************************************************************
 ethnet protocol
********************************************************************************************/
enum
{
    ETH_HEADER_LEN = 14,
    IPV4_VERSION   = 4,
    IPV6_VERSION   = 6,
    IPHDR_LEN      = 20,
    TCPHDR_LEN     = 20,
    UDPHDR_LEN     = 8
};

enum IP_LV4_PROTO
{
	LV4_TCP    = 6,  
	LV4_UDP    = 17,
}; 


struct IpEthHder
{
    BYTE DestMac[6];
    BYTE SrcMac[6];
    WORD EthType;
};


struct Ipv4Hdr  
{ 
	BYTE h_verlen;
	BYTE tos;            
	WORD total_len;      
	WORD ident;           
	WORD frag_and_flags;  
	BYTE ttl;            
	BYTE proto;           
	WORD checksum;        
	DWORD sourceIP;       
	DWORD destIP;       
};

struct TcpHdr 
{
	WORD  wSrcPort;
	WORD  wDstPort;
	DWORD dwSeqNum;
	DWORD dwAckNum; 
	BYTE  bHdrLen;
	BYTE  bTclFlg; 
	WORD  wWindowSize;
	WORD  wCheckSum; 
	WORD  wrgentPointer; 
};


struct UdpHdr 
{
	WORD wSrcPort;       
	WORD wDstPort;     
	WORD wLength ;      
	WORD wCheckSum;    
};

class IpPacket:public Packet
{
public:
    DWORD m_SrcIp;
    DWORD m_DstIp;
    WORD  m_SrcPort;
    WORD  m_DstPort;
    DWORD m_ProtoType;

    DWORD m_PayloadLen;
    BYTE* m_Payload;

private:
    IpEthHder* EthParse ();
    DWORD Ipv4Parse (IpEthHder* EthHeader);
    DWORD ParsePacket();

    set<DWORD>* m_UserIpSet;

    inline bool IsUserIp (DWORD Ip)
    {
        if (m_UserIpSet->find (Ip) != m_UserIpSet->end())
        {
            return true;
        }

        return false;
    }
    
public:
    IpPacket (BYTE* PktData, DWORD PktLen, set<DWORD>* UserIpSet):Packet(PktData, PktLen)
    {
        m_UserIpSet = UserIpSet;
        ParsePacket();
    }
};


#endif


