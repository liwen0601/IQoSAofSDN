#include <signal.h>
#include "bandwidth.h"

static void SigAlarm() 
{ 
    exit(0); 
}


static inline void Handshake (int Socket)
{
    socklen_t SockLen = sizeof(struct sockaddr_in);
    struct sockaddr_in ClientAddr;
    char Message[BUFSIZE] = {0};

    while (1)
    {
        int RecvBytes = recvfrom(Socket, Message, sizeof(Message), 0, (struct sockaddr*)&ClientAddr, &SockLen);
        if (RecvBytes < 0)
        {
            continue;
        }
        printf ("===> Recv %s \r\n", Message);
        
        strncpy (Message, "performance test response.", sizeof(Message));
        int SendBytes = sendto(Socket, Message, strlen(Message)+1, 0, (struct sockaddr*)&ClientAddr, SockLen);
        if (SendBytes < 0)
        {
            continue;
        }
        printf ("===> Send %s \r\n", Message);
       
        break;
    }
    
    return;
}


static inline void Perform (int Socket)
{
    socklen_t SockLen = sizeof (struct sockaddr_in);
    struct sockaddr_in ClientAddr;
    char Message[BUFSIZE] = {0};

    long RecvMsgNum = 0;
    long RecvBytes = 0;

    struct timeval StartTime;
    struct timeval EndTime;

    signal(SIGALRM, SigAlarm); 
    alarm(15); 

    int PerCount = 0;
    while (PerCount < PERFORMANCE_COUNT)
    {
        RecvBytes += recvfrom(Socket, Message, sizeof(Message), 0, (struct sockaddr*)&ClientAddr, &SockLen);        
        RecvMsgNum++;

        if (RecvMsgNum == 1)
        {
            gettimeofday(&StartTime, NULL);
        }
        else
        {
            gettimeofday(&EndTime, NULL);
            float TimeDiff = (EndTime.tv_sec - StartTime.tv_sec)*1.0 + (EndTime.tv_usec - StartTime.tv_usec)/1000000.0;
            if (TimeDiff < 1)
            {
                continue;
            }
            
            printf ("===> [[TimeDiff:%0.2f s] Recv %ld bytes, %ld packets, (%ld)], Bandwidth: [%0.2f Mbps]\r\n",
                    TimeDiff,
                    RecvBytes, 
                    RecvMsgNum,
                    RecvBytes/RecvMsgNum,
                    (RecvBytes*8.0)/TimeDiff/1024/1024);

            gettimeofday(&StartTime, NULL);
            RecvBytes = RecvMsgNum  =0;
            PerCount++;
        }
    }

    return;
}

void BandwidthTest (int Socket)
{
    /* handshake */
    Handshake (Socket);

    /* do performance test */
    Perform (Socket);       
}


