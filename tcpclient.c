/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2019 */     

#include "tcpHead.h"
#define STRING_SIZE 1024

int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE]; /* Server's hostname */
   unsigned short server_port;  /* Port number used by server (remote port) */

   char filename[LINELEN];  /* Buffer from which filename is sent */
   char recv_buffer[LINELEN]; /* Buffer into which the line is received */
  //  char modifiedSentence[LINELEN]; /* receive message */
  // unsigned int msg_len;  /* length of message */                      
   int bytes_sent = 0;
   int bytes_recd = 0; /* number of bytes sent or received */
  
   int filenameCnt;	/* Length of filename */
   struct packetHeader header; /* Struct from which header is sent */

   FILE *fp;

   /* open a socket */
   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   printf("Enter hostname of server: ");
   scanf("%s", server_hostname);
   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   printf("Enter port number for server: ");
   scanf("%hu", &server_port);

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

    /* connect to the server */
 		
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
  
   /* user interface */

   printf("Please input a filename:\n");
   scanf("%s", filename); // get file name from input

   /* Construct the header of the packet */
   filenameCnt = strlen(filename) + 1;		// gets payload size
   header.payload = htonl(filenameCnt);		// sets payload size
   header.packetSeqNum = htonl(1);		// sets packet sequence num
   send(sock_client, &header, HEADERSIZE, 0); /* Send header */
   send(sock_client, filename, filenameCnt,0); /* Send filename */

   fp = fopen(OUT_FILE, "w");		/* Open file */

   recv(sock_client, &header, 4,0);	/* Receive the first header */

   bytes_recd += recv(sock_client, recv_buffer,ntohl(header.payload), 0); /* Receive next Line */
	
   while (header.payload) {
	fputs (recv_buffer, fp); // Write to output file
	
	printf("Packet %d received with %d data bytes\n",
            ntohl(header.packetSeqNum), ntohl(header.payload));

	recv(sock_client, &header, 4, 0);	/* Receive next header */
	bytes_recd += recv(sock_client, recv_buffer,ntohl(header.payload), 0); /* Receive next line */
   }
   printf("End of transmission packet with sequence number %u received with "\
          "%u data bytes\n", ntohl(header.packetSeqNum),
          ntohl(header.payload));
   printf("Number of data packets received: %u\n", 
          ntohs(header.packetSeqNum) - 1);
   printf("Total number of data bytes received: %u\n", bytes_recd);

   /* close the socket */

   fclose(fp);	/* Close file */
   close (sock_client); 
}
