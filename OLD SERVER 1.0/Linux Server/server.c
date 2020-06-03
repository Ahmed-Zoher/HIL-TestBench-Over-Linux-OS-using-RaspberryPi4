/**
 *  @file server.c
 *  @author May Alaa
 *  @brief Linux Server Interfaces
 */
 
#include "server.h"

/********** LINUX SERVER INTERFACES ***********/

/**
 *  @brief This API shall initialize the Linux UDP link.
 *  
 *  @param [in] sockfd   Socket number
 *  @param [in] servaddr Server address
 *  @param [in] cliaddr  Client address
 *  @return void 
 */
void UDP_ServerInit(uint32_t *ServerSocket, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr)
{
	uint32_t keyFrame = 0;
	uint8_t ConnectionStatus = 0;
	uint32_t len = sizeof(struct sockaddr_in);
	
	// Creating socket file descriptor 
    if ( (*ServerSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{ 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
	
    printf("Socket created.\n");
    memset(servaddr, 0, sizeof(struct sockaddr_in)); 
    memset(cliaddr, 0, sizeof(struct sockaddr_in)); 
    
    // Filling server information 
    servaddr->sin_family    = AF_INET; // IPv4 
    servaddr->sin_addr.s_addr = inet_addr(SERVER); 
    servaddr->sin_port = htons(PORT); 
    
    // Bind the socket with the server address 
    if ( bind(*ServerSocket, (const struct sockaddr *)servaddr, sizeof(struct sockaddr_in)) < 0 ) 
    { 
        perror("bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Bind done\n");  
    
    while(1)
    {
		printf("Waiting for Initialization key....\n");
		
		UDP_ServerReceive(ServerSocket, (uint8_t*)&keyFrame, cliaddr, &len, sizeof(uint32_t));
		
		if(keyFrame == CONNECTION_KEY)
		{
			printf("CONNECTION KEY SUCCESS....\n");
			ConnectionStatus = ACK;
			UDP_ServerSend(ServerSocket, (uint8_t*)&ConnectionStatus, cliaddr, len, sizeof(uint8_t));
			break;
		}
		else
		{
			printf("CONNECTION KEY FAILURE....\n");
			ConnectionStatus = NACK;
			UDP_ServerSend(ServerSocket, (uint8_t*)&ConnectionStatus, cliaddr, len, sizeof(uint8_t));
		}
	}	
}

/**
 *  @brief This API shall send an acknoledgement upon receiving data.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_ServerSend(uint32_t *ServerSocket, uint8_t* Buffer, struct sockaddr_in * cliaddr, uint32_t len, uint16_t FrameSize)
{
	sendto(*ServerSocket, Buffer, FrameSize, MSG_CONFIRM, (const struct sockaddr *)cliaddr, len); 
}

/**
 *  @brief This API shall recieve a data frame.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] frame   The frame being received
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_ServerReceive(uint32_t *ServerSocket, uint8_t* frame, struct sockaddr_in*  cliaddr, uint32_t* len, uint16_t FrameSize)
{	
	recvfrom(*ServerSocket, (uint8_t*)frame, FrameSize,  MSG_WAITALL, (struct sockaddr *)cliaddr, len);
}

/**
 *  @brief This API shall terminate the Linux Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_ServerDisconnect(uint32_t *ServerSocket)
{
	close(*ServerSocket);
}

