/***********************************************************
 * Author: Wen Li
 * Date  : 4/02/2020
 * Describe: dpi
 * History:
   <1> 4/02/2020 , create
************************************************************/
#include "Server.h"
#include "Shell.h"


VOID Help ()
{
    printf("************************************************\r\n");
    printf("*                help information              *\r\n");
    printf("************************************************\r\n");
    printf("-p <listen port> \r\n");
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
        UserIp.insert (ntohl(inet_addr("192.168.159.130")));
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


void *DpiAnalysis (void* Arg)
{
    IpPacket * Pkt;
    ClassifyEngine *CfEngine = (ClassifyEngine *)Arg;

    while (1)
    {
        
        CfEngine->Analysis ();   
    }

    return NULL;
}

void *ShellThread (void* Arg)
{
    string Comand;
    CmdShell *Shell = (CmdShell*)Arg;

    while (1)
    {
        cout<<"shell>";
        cin >> Comand;

        Shell->ExeCmd (Comand);
    }
    
    return NULL;
}


int main(int argc, char *argv[])
{
    char ch;
    DWORD Port = 9163;
    pthread_t Tid;
    
    while((ch = getopt(argc, argv, "p:")) != -1)
    {
        switch(ch)
        {
            case 'p':
            {
                Port = atoi(optarg);
                if (Port == 0)
                {
                    return 0;
                }
                break;
            }
            default:
            {
                Help ();
                return 0;
            }
        }
    }

    PacketSet PtSet;
    T_IPSet *IpSet = GetUserIpSet ();

    ClassifyEngine CfEngine (&PtSet);
    int Ret = pthread_create(&Tid, NULL, DpiAnalysis, &CfEngine);
    assert (Ret == 0);

    CmdShell Shell (&CfEngine);
    Ret = pthread_create(&Tid, NULL, ShellThread, &Shell);
    assert (Ret == 0);
    
    TCPserver Server (Port, &PtSet, IpSet, &CfEngine);

    Server.Start ();
	
	return 0;
}

