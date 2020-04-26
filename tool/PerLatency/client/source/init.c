#include "init.h"

int InitSocket (char *Host, short Port, struct sockaddr_in *Addr)
{
	struct hostent *Hp;
	struct sockaddr_in AddrIn;
	int Socket;
   
	/* translate host name into peer’s IP address */
	Hp = gethostbyname(Host);
	if (!Hp) 
    {
		fprintf(stderr, "Unknown host: %s\n", Host);
		return -1;
	}

	/* build address data structure */
	bzero((char *)&AddrIn, sizeof(AddrIn));
	AddrIn.sin_family = AF_INET;
	bcopy(Hp->h_addr, (char *)&AddrIn.sin_addr, Hp->h_length);
	AddrIn.sin_port = htons(Port);

	/* active open */
	if ((Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) 
    {
		perror("socket() error");
		return -1;
	}

    *Addr = AddrIn;
	return Socket;
}

