/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _SERVER_H_
#define _SERVER_H_
#include <BasicType.h>
#include <Log.h>
#include <Context.h>

using namespace std;

class TCPserver
{
private:
    int m_Socket;
    struct sockaddr_in m_AddrIn;

    PacketSet *m_PacketSet;
    T_IPSet *m_UserIpSet;
    ClassifyEngine *m_CfEngine;

private:
    VOID Init (DWORD Port);
    
public:
    TCPserver(DWORD Port, PacketSet *PktSet, 
                T_IPSet *UserIpSet, ClassifyEngine *CfEngine)
    {
        m_PacketSet = PktSet;
        m_UserIpSet = UserIpSet;
        m_CfEngine  = CfEngine;

        Init (Port);
    }

    inline PacketSet* GetPktSet ()
    {
        return m_PacketSet;
    }

    inline T_IPSet* GetIpSet ()
    {
        return m_UserIpSet;
    }

    inline ClassifyEngine* GetCfEngine ()
    {
        return m_CfEngine;
    }

    DWORD Start();
};


struct Proxy
{
    Proxy (TCPserver *Server, int Socket)
    {
        m_Server = Server;
        m_Socket = Socket;
    }
    
    TCPserver *m_Server;
    int m_Socket;
};

#endif 
