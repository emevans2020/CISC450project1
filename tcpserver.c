/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* Sept. 19, 2019 */    

#include <ctype.h>          /* for toupper */
#include "tcpHead.h" 

#define STRING_SIZE 1024   

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 45690
int main(void) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char filename[LINELEN]; /* Buffer for file sent*/
   char recv_buffer[LINELEN]; /* receive message buffer  */

   struct packetHeader header;

   int filenameCnt;     /* Length of filename */
   int lineLen;  /* length of line */ 
   int read;   /* number of bytes read */
   int packetCnt = 0; /* Number of packets sent */

   int bytes_sent = 0;
   int bytes_recd = 0; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */

   FILE *fp;
   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
    
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof(client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   bytes_sent, packetCnt = 0;
   sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n"); 
         close(sock_server);
         exit(1);
      }
 
      /* receive the file name header from client */

      bytes_recd = recv(sock_connection, &header, sizeof(header), 0);

      if (bytes_recd > 0){ 
         filenameCnt = ntohl (header.payload);
	 memset(&header,0,sizeof(header));
      }
/* Receive file name data from the client */
	bytes_recd = recv(sock_connection, filename, LINELEN, 0);;
        if (bytes_recd > 0){
	       printf("Filename of length %d received: %s\n", filenameCnt,filename);
	
	fp = fopen(filename, "r"); // Attempt to open desired file
	if (fp == NULL) {
		perror("Server: file not found\n");
		close(sock_connection);
		exit(1);
	}
	 /* prepare the message to send */

	while (fgets(recv_buffer, sizeof(recv_buffer),fp)) {
		header.packetSeqNum = htonl(++packetCnt); // Write sequence number
		header.payload = htonl(strlen(recv_buffer) +1); //Write line length
		send(sock_connection, &header, 4, 0); //Send header

		bytes_sent += send(sock_connection,recv_buffer,ntohl(header.payload),0); // Send line

		printf ("Packet %d transmitted with %ld data bytes\n", packetCnt, strlen(recv_buffer)+1);
	}

		header.packetSeqNum = htonl(packetCnt+1); // Create and send
		header.payload = htonl(0);		// end of transmission
		send(sock_connection, &header, 4, 0);	// packet

		printf("End of Transmission Packet with sequence number %d transmitted with %d data bytes\n", packetCnt,0);
	
	fclose(fp); /* Close File */
	}

	printf ("Number of data packets transmitted: %u\n", packetCnt);
	printf ("Total number of data bytes transmitted: %u\n", bytes_sent);

      /* close the socket */

      close(sock_connection);
}
