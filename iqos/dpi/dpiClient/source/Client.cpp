/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2020
 * Describe: dpi client
 * History:
   <1> 4/05/2020 , create
************************************************************/
#include "Client.h"

VOID TCPclient::Init (string Host, DWORD Port)
{
    bzero((char *)&m_AddrIn, sizeof(m_AddrIn));
	m_AddrIn.sin_family = AF_INET;
	m_AddrIn.sin_port = htons(Port);
    m_AddrIn.sin_addr.s_addr = inet_addr(Host.c_str());

    m_Socket = socket(PF_INET, SOCK_STREAM, 0);
	assert (m_Socket > 0);
    
    int Ret = connect(m_Socket, (sockaddr *)&m_AddrIn, sizeof(m_AddrIn));
    if (Ret < 0)
    {
        cout <<"Connect to "<<Host<<":"<<Port<<" fail...\r\n";
        exit(0);
    }

    printf ("Connect to:%s-%d success\r\n",\
             inet_ntoa(m_AddrIn.sin_addr), m_AddrIn.sin_port);
    return;
}


VOID TCPclient::SendPacket (BYTE* Packet, DWORD Length)
{
    long Bytes = send(m_Socket, Packet, Length, 0);
    if (Bytes > 0)
    {
        //printf ("Send packet, Length = %u\r\n", Length);
    }

    DWORD CfId = 0;
    recv (m_Socket, &CfId, sizeof(CfId),0);
    if (CfId != 0)
    {
        //printf ("Flow been set as %u\r\n", CfId);
    }

    return;
}

