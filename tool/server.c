#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("./server TCP/UDP port\n");
        return 0;
    }

    int port;
    sscanf(argv[2],"%d",&port);
    int s;
    
    if(strcmp(argv[1],"TCP")==0)
        s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    else if(strcmp(argv[1],"UDP")==0)
        s = socket(AF_INET,SOCK_DGRAM,0);
    else
    {
        printf("./server TCP/UDP port\n");
        return 0;
    }
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    bind(s,(struct sockaddr*)&addr,sizeof(addr));
    if(strcmp(argv[1],"TCP")==0)
    {
        listen(s,20);
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int client_sock = accept(s,(struct sockaddr*)&client_addr,&client_addr_size);
        printf("connected %d\n",client_sock);
        char buf[2000];
        long pkt_recv=0;
        time_t now,prev;
        now=time(NULL);
        prev=time(NULL);
        while(1)
        {
            now=time(NULL);
            if(now!=prev)
            {
                printf("Speed: %ld bps\n",pkt_recv*8);
                pkt_recv=0;
                prev=now;
            }
            pkt_recv+=recvfrom(client_sock,buf,2000,0, (struct sockaddr *)&client_addr,&client_addr_size);
        }
    }
    else
    {
        char buf[2000];
        long pkt_recv=0;
        time_t now,prev;
        now=time(NULL);
        prev=time(NULL);
        while(1)
        {
            now=time(NULL);
            if(now!=prev)
            {
                printf("Speed: %ld bps\n",pkt_recv*8);
                pkt_recv=0;
                prev=now;
            }
            socklen_t sock_len = sizeof(addr);
            pkt_recv+=recvfrom(s,buf,2000,0, (struct sockaddr *)&addr,&sock_len);
        }

    }
}
