#include "latency.h"

void LatencyTest (int Socket)
{
	socklen_t SockLen = sizeof (struct sockaddr_in);
    struct sockaddr_in ClientAddr;
    char SendMessage[] = "message for latency test response.";
    char RecvMessage[BUFSIZE] = {0};
    
    struct in_addr Addr;

    char ClientIp[32] = {0};
    while (1)
    {
        int Recv = recvfrom(Socket, RecvMessage, sizeof(RecvMessage), 0, (struct sockaddr*)&ClientAddr, &SockLen);
        
        if (ClientIp[0] == 0)
        {
            inet_ntop(AF_INET, &ClientAddr.sin_addr, ClientIp, sizeof(ClientIp));
        }
        printf ("Recv form %s, Message:%s\r\n", ClientIp, RecvMessage);
        
        if (strncmp (RecvMessage, MESSAGE_FIN, sizeof(MESSAGE_FIN)) == 0)
        {
            break;
        }

        printf ("SendMessage:%s\r\n", SendMessage);
        int Send = sendto(Socket, SendMessage, sizeof(SendMessage), 0, (struct sockaddr*)&ClientAddr, SockLen);
    }
    
    return;
}

