/**
 *  @file server.c
 *  @author May Alaa
 *  @brief Linux Server Interfaces
 */
 
#include "server.h"

/************* MACROS DEFINITIONS ************/
#define MAXLINE  1024 
#pragma comment(lib,"ws2_32") 	//Winsock Library

/********** LINUX SERVER INTERFACES ***********/

/**
 *  @brief This API shall initialize the Linux UDP link.
 *  
 *  @param [in] sockfd   Socket number
 *  @param [in] servaddr Server address
 *  @param [in] cliaddr  Client address
 *  @return void 
 */
void UDP_ServerInit(uint32_t *sockfd, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr)
{
	// Creating socket file descriptor 
    if ( (*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
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
    if ( bind(*sockfd, (const struct sockaddr *)servaddr, sizeof(struct sockaddr_in)) < 0 ) 
    { 
        perror("bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Bind done\n");  
}

/**
 *  @brief This API shall send an acknoledgement upon receiving data.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_SendACK(uint32_t *sockfd, struct sockaddr_in * cliaddr,uint32_t len)
{
	char *Server_msg = "ack"; 
	sendto(*sockfd, (const char *)Server_msg, strlen(Server_msg), MSG_CONFIRM, (const struct sockaddr *)cliaddr, sizeof(struct sockaddr));
    printf("Ack sent.\n");   
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
void UDP_ReceiveFrame(uint32_t *sockfd, Test_frame* frame, struct sockaddr_in*  cliaddr, uint32_t* len)
{	
	recvfrom(*sockfd, (Test_frame*)frame, MAXLINE,  MSG_WAITALL, (struct sockaddr *)cliaddr, len); 
    printf("ID: %d\nCMD: %d\n", frame->ID,frame->CMD); 	
}

/**
 *  @brief This API shall terminate the Linux Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_Disconnect(uint32_t *sockfd)
{
	close(*sockfd);
}
