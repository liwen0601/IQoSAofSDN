/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Packet parse
 * History:
   <1> 3/28/2020 , create
************************************************************/

#include <Packet.h>
#include <Log.h>


DWORD IpEthPacket::EthParse ()
{
    if (m_PktLen < ETH_HEADER_LEN)
    {
        DebugLog ("EthParse, m_PktLen = %u", m_PktLen);
        return M_FAIL;
    }
    
    m_EthHeader = (IpEthHder*)m_PktData;
    m_EthHeader->EthType = ntohs(m_EthHeader->EthType);

    if (m_EthHeader->EthType != ETH_IPV4)
    {
        DebugLog ("Ipv4 EthParse, Ethtype = %#x", m_EthHeader->EthType);
        return M_FAIL;
    }
    
    DebugLog ("EthParse, Type = %#x", m_EthHeader->EthType);

    return M_SUCCESS;
}


DWORD IpEthPacket::Ipv4Parse ()
{
    BYTE *IpData;

    if (m_PktLen < ETH_HEADER_LEN+IPHDR_LEN)
    {
        DebugLog ("Ipv4Parse, m_PktLen = %u", m_PktLen);
        return M_FAIL;
    }
    
    m_Ipv4Header = (Ipv4Hdr*)((BYTE*)m_EthHeader + ETH_HEADER_LEN);
    
    IpData = (BYTE *)m_Ipv4Header;
    if(IpData[0]&0x40)
	{
		DWORD SrcIp;
		DWORD DstIp;

		DWORD IpHdrLen = (m_Ipv4Header->h_verlen&0x0f)<<2;
		if(IpHdrLen < IPHDR_LEN)
		{
			return M_FAIL;
		}

		m_Ipv4Header->total_len = ntohs(m_Ipv4Header->total_len);
        if (m_Ipv4Header->total_len+ETH_HEADER_LEN > m_PktLen)
        {
            m_Ipv4Header->total_len = m_PktLen - ETH_HEADER_LEN;
        }

        m_Ipv4Header->destIP   = ntohl(m_Ipv4Header->destIP);
        m_Ipv4Header->sourceIP = ntohl(m_Ipv4Header->sourceIP);


		DWORD Proto = (DWORD)m_Ipv4Header->proto;
        switch(Proto)
    	{
    	case LV4_TCP:
    		{
                if (m_PktLen < ETH_HEADER_LEN+IPHDR_LEN+TCPHDR_LEN)
                {
                    DebugLog ("Ipv4Parse, LV4_TCP, m_PktLen = %u", m_PktLen);
                    return M_FAIL;
                }
                
    			m_TcpHdr = (TcpHdr*)((BYTE*)m_Ipv4Header + IpHdrLen);
                
                BYTE TcpHdrLen = (m_TcpHdr->bHdrLen&0xf0)>>2;
    			if(TcpHdrLen < TCPHDR_LEN)
    			{
    				return M_FAIL;
    			}

                m_TcpHdr->wSrcPort = ntohs(m_TcpHdr->wSrcPort);
    			m_TcpHdr->wDstPort = ntohs(m_TcpHdr->wDstPort);
    			m_TcpHdr->dwSeqNum = ntohl(m_TcpHdr->dwSeqNum);    						
    						
    			break;
    		}
    	case LV4_UDP:
    		{
                if (m_PktLen < ETH_HEADER_LEN+IPHDR_LEN+8)
                {
                    DebugLog ("Ipv4Parse, LV4_UDP, m_PktLen = %u", m_PktLen);
                    return M_FAIL;
                }
                
    			m_UdpHdr = (UdpHdr*)((BYTE*)m_Ipv4Header + IpHdrLen);

    			m_UdpHdr->wSrcPort = ntohs(m_UdpHdr->wSrcPort);
    			m_UdpHdr->wDstPort = ntohs(m_UdpHdr->wDstPort);						

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
    

DWORD IpEthPacket::ParsePacket()
{
    DWORD Ret = EthParse ();
    if (Ret != M_SUCCESS)
    {
        return Ret;
    }

    
    return Ipv4Parse ();	
}

