/***********************************************************
 * Author: Wen Li
 * Date  : 4/14/2020
 * Describe: DPI client, send packet to server
 * History:
   <1> 4/14/2020 , create
************************************************************/
#include "Capture.h"
#include "Client.h"


static TCPclient* Client = NULL;

VOID Help ()
{
    printf("************************************************\r\n");
    printf("*                help information              *\r\n");
    printf("************************************************\r\n");
    printf("-d <device name> \r\n");
    printf("-s <server ip>   \r\n");
    printf("-p <srver port>  \r\n");
    printf("************************************************\r\n\r\n");

    return;
}


static VOID Analysis(BYTE *user,  struct pcap_pkthdr *Hdr, BYTE *PktData)
{
    if (Client == NULL)
    {
        return;
    }
    
#define ETH_HEADER_LEN (14)

    if (Hdr->caplen < ETH_HEADER_LEN + 28)
    {
        return;
    }
    
    Client->SendPacket (PktData+ETH_HEADER_LEN, Hdr->caplen-ETH_HEADER_LEN);
    
	return;
}


int main(int argc, char *argv[])
{
    char ch;
    string Device = "";

    string ServerIp = "127.0.0.1";
    DWORD  ServerPort = 9163;
    
    while((ch = getopt(argc, argv, "d:s:p:")) != -1)
    {
        switch(ch)
        {
            case 'd':
            {
                Device = optarg;
                break;
            }
            case 's':
            {
                ServerIp = optarg;
                break;
            }
            case 'p':
            {
                ServerPort = atoi(optarg);
                break;
            }
            default:
            {
                Help ();
                return 0;
            }
        }
    }

    if (Device == "")
    {
        Help();
        return 0;
    }

    Client = new TCPclient (ServerIp, ServerPort);
    assert (Client != NULL);
        
    Capture Cap(Device);
    Cap.CapturePacket ((pcap_handler)Analysis);
	
	return 0;
}

