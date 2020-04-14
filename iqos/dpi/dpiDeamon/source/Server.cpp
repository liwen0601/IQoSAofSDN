/***********************************************************
 * Author: Wen Li
 * Date  : 4/05/2020
 * Describe: Classifier match
 * History:
   <1> 4/05/2020 , create
************************************************************/
#include "Log.h"
#include "Server.h"


VOID TCPserver::Init (DWORD Port) 
{
    bzero((char *)&m_AddrIn, sizeof(m_AddrIn));
    m_AddrIn.sin_family = AF_INET;
    m_AddrIn.sin_addr.s_addr = INADDR_ANY;
    m_AddrIn.sin_port = htons(Port);

    m_Socket = socket(PF_INET, SOCK_DGRAM, 0);
    assert (m_Socket > 0);

    int Ret = bind(m_Socket, (struct sockaddr *)&m_AddrIn, sizeof(m_AddrIn)); 
    assert (Ret > 0);

    Ret = listen(m_Socket, 10);
    assert (Ret > 0);
    
	return;
}


DWORD TCPserver::Start()
{
    socklen_t SockLen = sizeof (struct sockaddr_in);
    struct sockaddr_in ClientAddr;
    char Message[1500] = {0};

    while( 1 ) 
    {
        int Socket = accept(m_Socket, (struct sockaddr*)&ClientAddr, &SockLen);
        if(Socket < 0)
        {
            continue;
        }

        DebugLog ("Receive a connection\r\n");
        
        long RecvBytes = recv(Socket, Message, sizeof(Message), 0);
        if (RecvBytes == 0)
        {
            close (Socket);
            continue;
        }

        IpPacket *Ip = new IpPacket ((BYTE*)Message, RecvBytes, m_UserIpSet);
        assert (Ip == NULL);

        DWORD CfId = m_CfEngine->Query (Ip);
        if (CfId == 0)
        {
            m_PacketSet->Push (Ip);
        }
        else
        {
            DebugLog ("classify flow as: %d \r\n", CfId);
            send(Socket, &CfId, sizeof(CfId) , 0);
        }

        close (Socket);
    }
        

}

