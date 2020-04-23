#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    if(argc!=5)
    {
        printf("./client server_ip, server_port TCP/UDP *.pcapng\n");
        return 0;    
    }
    char ip[32];
    strncpy(ip,argv[1],32);
    int port;
    sscanf(argv[2],"%d",&port);

    char ebuf[PCAP_ERRBUF_SIZE];
    pcap_t *p = pcap_open_offline(argv[4],ebuf);
    struct pcap_pkthdr packet;
    unsigned char sendStr[500][2000];
    int len[500];
    int flowLen=0;
    for(flowLen=0;flowLen<500;flowLen++)
    {
        const unsigned char * pktStr = pcap_next(p,&packet);
        if(pktStr == 0)
            break;
        memcpy(sendStr[flowLen],pktStr,packet.len);
        len[flowLen]=packet.len;
    }
    pcap_close(p);
    if(strcmp("TCP",argv[3])==0)
    {
        
        int s = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);
        if(connect(s,(struct sockaddr*)&addr,sizeof(addr)))
            printf("connect failed\n");
        //send(s,"123",5,0);
    
        while(1)
        {
            for(int i=0;i<flowLen;i++)
            {   
                struct iphdr *hdr;
                hdr=(struct iphdr *)sendStr[i];
                sendto(s,sendStr[i]+54,len[i]-54,0,(struct sockaddr*)&addr,sizeof(addr));
                usleep(1);
            }
            //shutdown(s,SHUT_RDWR);
            //usleep(1000);
        /*printf("Packet length: %d\n", len[i]);
        for(int j=0;j<len[i];j++)
        {
            printf("%x  ",sendStr[i][j]);
        }
        printf("\n\n\n\n");
        */
        }
    }
    else
    {
        int s = socket(AF_INET,SOCK_DGRAM,0);
        struct sockaddr_in addr;
        memset(&addr,0,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);      
        while(1)
        {
            for(int i=0;i<flowLen;i++)
            {   
                struct iphdr *hdr;
                hdr=(struct iphdr *)sendStr[i];
                sendto(s,sendStr[i]+54,len[i]-54,0,(struct sockaddr*)&addr,sizeof(addr));
                usleep(1);
            }

        }
             
    }
}
