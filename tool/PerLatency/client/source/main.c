#include "init.h"
#include "latency.h"
#include "bandwidth.h"


int main(int argc, char * argv[])
{
	int Socket;
    char *Host;
    short Port;
    int Mode;
    struct sockaddr_in Addr;

	if (argc==4) 
    {
		Host = argv[1];
		Port = atoi(argv[2]);
		Mode = atoi(argv[3]);
	}
	else 
    {
		fprintf(stderr, "Usage: ./client host port mode\n");
		exit(1);
	}

    Socket = InitSocket (Host, Port, &Addr);
    if (Socket <= 0)
    {
        fprintf(stderr, "initialize socket fail...\n");
		exit(1);
    }
    
	
	if(Mode == 0)
    {
	    // Perform Bandwidth Test
	    BandwidthTest (Socket, &Addr);

	}
	else if(Mode == 1)
    {
        // Perform Latency Test
        LatencyTest (Socket, &Addr);

	}
	else
    {
	    printf("Error, incorrect mode\n");
	}
    
	close(Socket);

    return 0;
}

