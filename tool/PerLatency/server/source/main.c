#include "init.h"
#include "latency.h"
#include "bandwidth.h"


int main(int argc, char * argv[])
{
    int Socket;
    short Port;
    int Mode;
    struct sockaddr_in Addr;

    if (argc == 3) 
    {
        Port = atoi(argv[1]);
        Mode = atoi(argv[2]);
    }
    else 
    {
        fprintf(stderr, "usage: ./server port mode\n");
        exit(1);
    }

    Socket = InitSocket (Port, &Addr);
    if (Socket <= 0)
    {
        fprintf(stderr, "initialize socket fail...\n");
		exit(1);
    }
    
	
	if(Mode == 0)
    {
	    // Perform Bandwidth Test
	    BandwidthTest (Socket);

	}
	else if(Mode == 1)
    {
        // Perform Latency Test
        LatencyTest (Socket);

	}
	else
    {
	    printf("Error, incorrect mode\n");
	}
    
	close(Socket);

    return 0;
}
