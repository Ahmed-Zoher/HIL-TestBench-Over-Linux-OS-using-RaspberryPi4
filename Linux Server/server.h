#ifndef SERVER_H
#define SERVER_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

/************* MACROS DEFINITIONS ************/
#define PORT     		8080 
#define SERVER 			"192.168.5.10"
#define ACK_SIZE 		4
#define NUM_OF_FRAMES	3

/************* TYPES DEFINITIONS ************/
typedef struct{
	uint32_t ID;
	uint32_t CMD;
	uint8_t Ay7aga;
}Test_frame;

/**
 *  @brief This API shall initialize the Linux UDP link.
 *  
 *  @param [in] sockfd   Socket number
 *  @param [in] servaddr Server address
 *  @param [in] cliaddr  Client address
 *  @return void 
 */
void UDP_ServerInit(uint32_t *sockfd, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr);

/**
 *  @brief This API shall send an acknoledgement upon receiving data.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_ReceiveFrame(uint32_t *sockfd, Test_frame* frame, struct sockaddr_in* cliaddr, uint32_t *len);

/**
 *  @brief This API shall recieve a data frame.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] frame   The frame being received
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_SendACK(uint32_t *sockfd, struct sockaddr_in* cliaddr,uint32_t len);

/**
 *  @brief This API shall terminate the Linux Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_Disconnect(uint32_t *sockfd);

#endif /* SERVER_H */
