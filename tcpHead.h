#include <stdio.h>	/* for standard I/O functions */
#include <stdlib.h>	/* for exit */
#include <string.h>	/* for memset, memcpy, and strlen */
#include <netdb.h>	/* for struct hostent and gethostbyname */
#include <sys/socket.h> /* for socket, connect, send, and recv */
#include <netinet/in.h> /* for socket_addr_in */
#include <unistd.h>	/*  for close */

#define LINELEN 81	// No more than 80 characters on a line and includes newline char at end
#define HEADERSIZE 4	// Header size is 4 bytes in size
#define PACKETSIZE 85   // Max size of packet

#define OUT_FILE "out.txt"	// Name of tcpclient.c output file

struct packetHeader {
	int payload;	/* count of the number of data characters in the packet 0-80 */
	int packetSeqNum; /*each packet transmitted by the server is assigned a sequence
			    number starting with 1 and incremented by 1 for each packet */
};
	
