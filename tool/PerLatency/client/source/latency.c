#include "latency.h"

#define MSG_NUM (10)

void LatencyTest (int Socket, struct sockaddr_in *Addr)
{
    int MsgIndex = 0;
	socklen_t SockLen = sizeof(struct sockaddr_in);
    char SendMessage[] = "message for latency test request.";
    char RecvMessage[BUFSIZE] = {0};

    struct timeval SendTime;
    struct timeval RecvTime;
    float TimeElapsed = 0;
    float Diff = 0;

    char ServerIp[32] = {0};
    while (MsgIndex < MSG_NUM)
    {
        printf ("SendMessage:%s\r\n", SendMessage);
        sendto(Socket, SendMessage, strlen(SendMessage)+1, 0, (struct sockaddr*)Addr, SockLen);
        gettimeofday(&SendTime, NULL);

        int RecvSize = recvfrom(Socket, RecvMessage, sizeof(RecvMessage), 0, (struct sockaddr*)Addr, &SockLen);
        gettimeofday(&RecvTime, NULL);

        Diff = (RecvTime.tv_sec - SendTime.tv_sec)*1000 + (RecvTime.tv_usec - SendTime.tv_usec)/1000;
        TimeElapsed += Diff;

        if (ServerIp[0] == 0)
        {
            inet_ntop(AF_INET, &Addr->sin_addr, ServerIp, sizeof(ServerIp));
        }
        
        printf("receive %d bytes from %s: MsgSeq=%d time=%0.2f ms\r\n", RecvSize, ServerIp, MsgIndex+1, Diff);
        MsgIndex++;
    }

    printf ("===> Average latency is: %0.2f ms\r\n", TimeElapsed/MSG_NUM);
    sendto(Socket, MESSAGE_FIN, sizeof(MESSAGE_FIN), 0, (struct sockaddr*)Addr, SockLen);

    return;
}

