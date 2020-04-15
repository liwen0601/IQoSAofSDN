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

    m_Socket = socket(PF_INET, SOCK_STREAM, 0);
    assert (m_Socket > 0);

    int Ret = bind(m_Socket, (struct sockaddr *)&m_AddrIn, sizeof(m_AddrIn)); 
    assert (Ret >= 0);

    Ret = listen(m_Socket, 10);
    assert (Ret >= 0);
    
	return;
}

void *ProxyThread (void* Arg)
{
    char Message[1500];
    
    Proxy *P = (Proxy *)Arg;
    TCPserver *Srver = P->m_Server;
    int Socket = P->m_Socket;
    delete P;
    
    PacketSet* PktSet  = Srver->GetPktSet ();
    T_IPSet* UserIpSet = Srver->GetIpSet ();
    ClassifyEngine* CfEngine = Srver->GetCfEngine ();
     
    while (1)
    { 
        long RecvBytes = recv(Socket, Message, sizeof(Message), 0);
        if (RecvBytes == 0)
        {
            DebugLog ("Receive RecvBytes == 0\r\n");
            continue;
        }

        IpPacket *Ip = new IpPacket ((BYTE*)Message, RecvBytes, UserIpSet);
        assert (Ip != NULL);

        DWORD CfId = 0;
        if (Ip->m_SrcIp != 0)
        {
            CfId = CfEngine->Query (Ip);
            if (CfId == 0)
            {
                DebugLog ("Push packet: %p \r\n", Ip);
                PktSet->Push (Ip);
            }
        }
        else
        {
            delete Ip;            
        }
        
        DebugLog ("classify flow as: %d \r\n", CfId);
        send(Socket, &CfId, sizeof(CfId) , 0);
    }

    close (Socket);
    return NULL;
}



DWORD TCPserver::Start()
{
    pthread_t Tid;
    socklen_t SockLen = sizeof (struct sockaddr_in);
    struct sockaddr_in ClientAddr;

    while (1)
    {
        int Socket = accept(m_Socket, (struct sockaddr*)&ClientAddr, &SockLen);
        if(Socket < 0)
        {
            return M_FAIL;
        }

        DebugLog ("Receive a connection:%s-%d\r\n",\
                  inet_ntoa(ClientAddr.sin_addr), ClientAddr.sin_port);

        Proxy *P = new Proxy(this, Socket);
        int Ret = pthread_create(&Tid, NULL, ProxyThread, P);
        assert (Ret == 0);
    }
    

    return M_SUCCESS;
}

