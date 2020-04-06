/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Packet parse
 * History:
   <1> 3/28/2020 , create
************************************************************/
#include <Packet.h>
#include <Log.h>


IpEthHder* IpPacket::EthParse ()
{
    if (m_PktLen < ETH_HEADER_LEN)
    {
        DebugLog ("EthParse, m_PktLen = %u", m_PktLen);
        return NULL;
    }
    
    IpEthHder *EthHeader = (IpEthHder*)m_PktData;
    EthHeader->EthType = ntohs(EthHeader->EthType);

    if (EthHeader->EthType != ETH_IPV4)
    {
        DebugLog ("Ipv4 EthParse, Ethtype = %#x", EthHeader->EthType);
        return NULL;
    }
    
    DebugLog ("EthParse, Type = %#x", EthHeader->EthType);

    return EthHeader;
}


DWORD IpPacket::Ipv4Parse (IpEthHder* EthHeader)
{
    BYTE *IpData;

    if (m_PktLen < ETH_HEADER_LEN+IPHDR_LEN)
    {
        DebugLog ("Ipv4Parse, m_PktLen = %u", m_PktLen);
        return M_FAIL;
    }
    
    Ipv4Hdr* Ipv4Header = (Ipv4Hdr*)((BYTE*)EthHeader + ETH_HEADER_LEN);
    
    IpData = (BYTE *)Ipv4Header;
    if(IpData[0]&0x40)
	{
		DWORD SrcIp;
		DWORD DstIp;

		DWORD IpHdrLen = (Ipv4Header->h_verlen&0x0f)<<2;
		if(IpHdrLen < IPHDR_LEN)
		{
			return M_FAIL;
		}

		Ipv4Header->total_len = ntohs(Ipv4Header->total_len);
        if (Ipv4Header->total_len+ETH_HEADER_LEN > m_PktLen)
        {
            Ipv4Header->total_len = m_PktLen - ETH_HEADER_LEN;
        }

        Ipv4Header->destIP   = ntohl(Ipv4Header->destIP);
        Ipv4Header->sourceIP = ntohl(Ipv4Header->sourceIP);

		DWORD Proto = (DWORD)Ipv4Header->proto;
        switch(Proto)
    	{
    	case LV4_TCP:
    		{
                if (m_PktLen < ETH_HEADER_LEN+IPHDR_LEN+TCPHDR_LEN)
                {
                    DebugLog ("Ipv4Parse, LV4_TCP, m_PktLen = %u", m_PktLen);
                    return M_FAIL;
                }
                
    			TcpHdr* THdr = (TcpHdr*)((BYTE*)Ipv4Header + IpHdrLen);
                
                BYTE TcpHdrLen = (THdr->bHdrLen&0xf0)>>2;
    			if(TcpHdrLen < TCPHDR_LEN)
    			{
    				return M_FAIL;
    			}

                THdr->wSrcPort = ntohs(THdr->wSrcPort);
    			THdr->wDstPort = ntohs(THdr->wDstPort);
    			THdr->dwSeqNum = ntohl(THdr->dwSeqNum);

                m_ProtoType = LV4_TCP;
                m_PayloadLen = m_PktLen-(ETH_HEADER_LEN+IPHDR_LEN+TcpHdrLen);
                
                if (IsUserIp (Ipv4Header->sourceIP))
                {
                    m_SrcIp = Ipv4Header->sourceIP;
                    m_DstIp = Ipv4Header->destIP;

                    m_SrcPort = THdr->wSrcPort;
                    m_DstPort = THdr->wDstPort;
                }
                else
                {
                    m_SrcIp = Ipv4Header->destIP;
                    m_DstIp = Ipv4Header->sourceIP;

                    m_SrcPort = THdr->wDstPort;
                    m_DstPort = THdr->wSrcPort;
                }
    						
    			break;
    		}
    	case LV4_UDP:
    		{
                if (m_PktLen < ETH_HEADER_LEN+IPHDR_LEN+UDPHDR_LEN)
                {
                    DebugLog ("Ipv4Parse, LV4_UDP, m_PktLen = %u", m_PktLen);
                    return M_FAIL;
                }
                
    			UdpHdr *UHdr = (UdpHdr*)((BYTE*)Ipv4Header + IpHdrLen);

    			UHdr->wSrcPort = ntohs(UHdr->wSrcPort);
    			UHdr->wDstPort = ntohs(UHdr->wDstPort);

                m_ProtoType = LV4_UDP;
                m_PayloadLen = m_PktLen-(ETH_HEADER_LEN+IPHDR_LEN+UDPHDR_LEN);
                
                if (IsUserIp (Ipv4Header->sourceIP))
                {
                    m_SrcIp = Ipv4Header->sourceIP;
                    m_DstIp = Ipv4Header->destIP;

                    m_SrcPort = UHdr->wSrcPort;
                    m_DstPort = UHdr->wDstPort;
                }
                else
                {
                    m_SrcIp = Ipv4Header->destIP;
                    m_DstIp = Ipv4Header->sourceIP;

                    m_SrcPort = UHdr->wDstPort;
                    m_DstPort = UHdr->wSrcPort;
                }

    			break;
    		}
    	default:
    		{
    			break;
    		}
    	}


		return M_SUCCESS;

	}
	else if(IpData[0]&0x60)
	{
		/* ipv6 */
        DebugLog ("Ipv6 packets, not support..");
		return M_FAIL;
	}
	else
	{
        DebugLog ("Unsupport protocol: %#x", (DWORD)IpData[0]);
		return M_FAIL;
	}
     
}
    

DWORD IpPacket::ParsePacket()
{
    IpEthHder* EthHeader = EthParse ();
    if (EthHeader == NULL)
    {
        return M_FAIL;
    }

    
    return Ipv4Parse (EthHeader);	
}

