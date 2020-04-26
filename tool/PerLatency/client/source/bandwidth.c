#include "bandwidth.h"

static inline void Handshake (int Socket, struct sockaddr_in *Addr)
{
    char Message[BUFSIZE] = {0};
    socklen_t SockLen = sizeof(struct sockaddr_in);

    while (1)
    {
        strncpy (Message, "performance test request.", sizeof(Message));
        int SendBytes = sendto(Socket, Message, strlen(Message)+1, 0, (struct sockaddr*)Addr, SockLen);
        if (SendBytes < 0)
        {
            continue;
        }
        printf ("===> Send %s \r\n", Message);

        int RecvBytes = recvfrom(Socket, Message, sizeof(Message), 0, (struct sockaddr*)Addr, &SockLen);
        if (SendBytes < 0)
        {
            continue;
        }

        printf ("===> Recv %s \r\n", Message);
        break;
    }
    
    return;
}

static inline void Perform (int Socket, struct sockaddr_in *Addr)
{
    char Message[BUFSIZE] = {0};
    socklen_t SockLen = sizeof(struct sockaddr_in);
    
    struct timeval StartTime;
    struct timeval EndTime;
  
    long SendBytes = 0;
    long SendMsgNum = 0;


    int PerCount = 0;
    while (PerCount < PERFORMANCE_COUNT)
    {
        SendBytes += sendto(Socket, Message, sizeof(Message), 0, (struct sockaddr*)Addr, SockLen);
        SendMsgNum++;

        if (SendMsgNum == 1)
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

            printf ("===>[%0.2f s]Send %ld bytes, (%ld) packets (%ld), rate:%0.2f\r\n", 
                    TimeDiff, SendBytes, SendMsgNum, SendBytes/SendMsgNum, SendBytes*8.0/TimeDiff/1024/1024);

            SendMsgNum = SendBytes = 0;
            gettimeofday(&StartTime, NULL);
            PerCount++;
        }
    }
    
    return;
}

void BandwidthTest (int Socket, struct sockaddr_in *Addr)
{    
    /* handshake */
    Handshake (Socket, Addr);

    /* do performance test */
    Perform (Socket, Addr);
    
	return;
}

