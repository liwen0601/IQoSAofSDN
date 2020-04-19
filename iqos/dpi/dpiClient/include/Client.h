/***********************************************************
 * Author: Wen Li
 * Date  : 3/28/2020
 * Describe: Context for flow analysis
 * History:
   <1> 3/28/2020 , create
************************************************************/

#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <BasicType.h>
#include <Log.h>

#define MAX_PKT_NUM  (256)
#define PKT_LEN      (1500)


using namespace std;

class TCPclient
{
private:
    int m_Socket;
    struct sockaddr_in m_AddrIn;

private:

    VOID Init (string Host, DWORD Port);
    
public:
    TCPclient(string Host, DWORD Port)
    {
        m_Socket = 0;
        Init (Host, Port);
    }

    ~TCPclient()
    {
        close (m_Socket);
    }

    VOID SendPacket (BYTE* Packet, DWORD Length);
};



#endif 
