/***********************************************************
 * Author: Wen Li
 * Date  : 4/02/2020
 * Describe: dos attack analysis 
 * History:
   <1> 4/02/2020 , create
************************************************************/
#include "Capture.h"

VOID Help ()
{
    printf("************************************************\r\n");
    printf("*                help information              *\r\n");
    printf("************************************************\r\n");
    printf("-d device name \r\n");
    printf("************************************************\r\n\r\n");

    return;
}




static VOID Analysis(BYTE *user,  struct pcap_pkthdr *Hdr, BYTE *PktData)
{
    
	return;
}


int main(int argc, char *argv[])
{
    char ch;
    string Device = "";
    while((ch = getopt(argc, argv, "d:")) != -1)
    {
        switch(ch)
        {
            case 'd':
            {
                Device = optarg;
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
        
    Capture Cap(Device);
    Cap.CapturePacket ((pcap_handler)Analysis);
	
	return 0;
}

