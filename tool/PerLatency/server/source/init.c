#include "init.h"

int InitSocket (short Port, struct sockaddr_in *Addr)
{
	struct sockaddr_in AddrIn;
	int Socket;
   
	 /* build address data structure */
     bzero((char *)&AddrIn, sizeof(AddrIn));
     AddrIn.sin_family = AF_INET;
     AddrIn.sin_addr.s_addr = INADDR_ANY;
     AddrIn.sin_port = htons(Port);

     /* setup passive open */
     if ((Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) 
     {
        perror("socket() error");
        return -1;
     }

     if ((bind(Socket, (struct sockaddr *)&AddrIn, sizeof(AddrIn))) < 0) 
     {
        perror("bind() error");
        return -1;
     }

    *Addr = AddrIn;
	return Socket;
}

