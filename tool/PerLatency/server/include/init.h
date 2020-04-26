
#ifndef _INIT_H_
#define _INIT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>


int InitSocket (short Port, struct sockaddr_in *Addr);

#endif
