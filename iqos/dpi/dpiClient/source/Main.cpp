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
BYTE   PktData[MAX_PKT_NUM][PKT_LEN];
DWORD  PktLen[MAX_PKT_NUM];


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

static inline T_IPSet* GetUserIpSet ()
{
    char IPaddr[128];
    static T_IPSet UserIp;
    
    FILE *F = fopen (USER_IP_PROPERTITY, "r");
    if (F == NULL)
    {
        printf ("No user ip configured!\r\n");
        exit (0);
    }
    else
    {
        while (!feof(F))
        {
            char *Ret = fgets (IPaddr, sizeof (IPaddr), F);
            if (Ret == NULL)
            {
                break;
            }

            UserIp.insert (ntohl(inet_addr(IPaddr)));
        }

        fclose (F);
    }

    return &UserIp;
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


static DWORD PktNum = 0;
static VOID ReadPcap(BYTE *temp1, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
    if (PktNum < MAX_PKT_NUM)
    {
        PktLen[PktNum] = (header->caplen > PKT_LEN)?(PKT_LEN):(header->caplen); 
    	memcpy(PktData[PktNum], (BYTE*)pkt_data+14, PktLen[PktNum]);	
    }

    PktNum++;
	
	return;
}

bool Prepare(CHAR* PcapFile)
{
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];

    if (PcapFile == NULL)
    {
        return false;
    }
	
	fp = pcap_open_offline(PcapFile, errbuf);
	if (NULL == fp)
	{
		printf("pcap_open_offline fail, pbPcapFile=%s\r\n", PcapFile);
		return false;
	}
	
	pcap_loop(fp, 0, ReadPcap, NULL);	
	pcap_close(fp);

	printf("===============================\r\n");
	printf("get packet num is %u \r\n", PktNum);
	printf("===============================\r\n");

	return true;
}


VOID PerfTest(CHAR* UserIp, bool IsPerf)
{
    DWORD NewDstIp = 1;
    DWORD UIp = 0;
    if (UserIp != NULL)
    {
        UIp = inet_addr(UserIp);
    }

    T_IPSet* IpSet = GetUserIpSet ();
    assert (IpSet != NULL);
    
    while (1)
    {
        DWORD Index = 0;
        
        while (Index < PktNum)
        {
            Ipv4Hdr* Ipv4Header = (Ipv4Hdr*)PktData[Index];
            if (IsPerf)
            {
                DWORD SrcIp = ntohl (Ipv4Header->sourceIP);
                DWORD DstIp = ntohl (Ipv4Header->destIP);

                if (IpSet->find (SrcIp) != IpSet->end())
                {
                    Ipv4Header->destIP = NewDstIp;
                    if (UIp)
                    {
                        Ipv4Header->sourceIP = UIp;
                    }
                }
                else if (IpSet->find (DstIp) != IpSet->end())
                {
                    Ipv4Header->sourceIP = NewDstIp;
                    if (UIp)
                    {
                        Ipv4Header->destIP   = UIp;
                    }
                }
                else
                {
                    printf ("src:%u, dst:%u \r\n", Ipv4Header->sourceIP, Ipv4Header->destIP);
                    break;
                }
            }
            
            
            Client->SendPacket ((BYTE*)Ipv4Header, PktLen[Index]);

            Index++;
        }

        NewDstIp++;
        if (!IsPerf)
        {
            break;
        }
    }
}


int main(int argc, char *argv[])
{
    char ch;
    string Device = "";

    string ServerIp = "127.0.0.1";
    DWORD  ServerPort = 9163;

    CHAR* PcapFile = NULL;
    bool IsPerf = false;
    CHAR* UserIp = NULL;
    
    while((ch = getopt(argc, argv, "d:s:p:xf:a:")) != -1)
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
            case 'x':
            {
                IsPerf = true;
                break;
            }
            case 'f':
            {
                PcapFile = optarg;
                break;
            }
            case 'a':
            {
                UserIp = optarg;
                break;
            }
            default:
            {
                Help ();
                return 0;
            }
        }
    }

    if (PcapFile)
    {
        if (!Prepare (PcapFile))
        {
            return 0;
        }
        
        Client = new TCPclient (ServerIp, ServerPort);
        assert (Client != NULL);

        PerfTest(UserIp, IsPerf);
    }
    else
    {
        if (Device == "")
        {
            Help();
            return 0;
        }

        Client = new TCPclient (ServerIp, ServerPort);
        assert (Client != NULL);
            
        Capture Cap(Device);
        Cap.CapturePacket ((pcap_handler)Analysis);
    }
	
	return 0;
}

