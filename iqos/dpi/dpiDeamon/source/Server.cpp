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
    char Message[PACKET_SIZE];
    
    Proxy *P = (Proxy *)Arg;
    TCPserver *Srver = P->m_Server;
    int Socket = P->m_Socket;
    delete P;
    
    PacketSet* PktSet  = Srver->GetPktSet ();
    T_IPSet* UserIpSet = Srver->GetIpSet ();
    ClassifyEngine* CfEngine = Srver->GetCfEngine ();
     
    while (1)
    { 
        long RecvBytes = recv(Socket, Message, sizeof(Message), MSG_NOSIGNAL);
        if (RecvBytes < 0)
        {
            DebugLog ("Receive error occur, exit current thread...\r\n");
            break;
        }

        IpPacket *Ip = new IpPacket ((BYTE*)Message, RecvBytes, UserIpSet);
        if (Ip == NULL)
        {
            DebugLog ("Allot a packet fail...\r\n");
            continue;
        }

        DWORD CfId = 0;
        if (Ip->m_SrcIp != 0)
        {
            Flow *Fctx = CfEngine->QueryFlow (Ip);
            CfId = Fctx->GetCfId ();
            
            if (Fctx->m_SduNum <= DPI_NUM && (CfId == 0) && (Ip->m_Payload > 0))
            {
                DebugLog ("Push packet: %p [%u]\r\n", Ip, PktSet->Size ());
                PktSet->Push (Ip);
            }
            else
            {
                Fctx->m_PacketNum++;
                CfEngine->UpdateStatistic (Ip);
                delete Ip;
            }
        }
        else
        {
            delete Ip;            
        }
        
        DebugLog ("classify flow as: %d \r\n", CfId);
        long SendBytes = send(Socket, &CfId, sizeof(CfId) , MSG_NOSIGNAL);
        if (SendBytes <= 0)
        {
            DebugLog ("Send error occur, exit current thread...\r\n");
            break;
        }
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

