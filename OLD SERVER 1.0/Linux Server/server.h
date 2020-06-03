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
#define PORT     		8888 
#define SERVER 			"192.168.5.10"
#define ACK 			0x05
#define NACK			0x08
#define STATUS_SIZE		1
#define NUM_OF_FRAMES	3

#define CONNECTION_KEY	0x12487

/**
 *  @brief This API shall initialize the Linux UDP link.
 *  
 *  @param [in] sockfd   Socket number
 *  @param [in] servaddr Server address
 *  @param [in] cliaddr  Client address
 *  @return void 
 */
void UDP_ServerInit(uint32_t *ServerSocket, struct sockaddr_in *servaddr, struct sockaddr_in *cliaddr);

/**
 *  @brief This API shall send an acknoledgement upon receiving data.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_ServerSend(uint32_t *ServerSocket, uint8_t* Buffer, struct sockaddr_in * cliaddr, uint32_t len, uint16_t FrameSize);

/**
 *  @brief This API shall recieve a data frame.
 *  
 *  @param [in] sockfd  Socket number
 *  @param [in] frame   The frame being received
 *  @param [in] cliaddr Client address
 *  @param [in] len     Length of data being received
 *  @return void
 */
void UDP_ServerReceive(uint32_t *ServerSocket, uint8_t* frame, struct sockaddr_in*  cliaddr, uint32_t* len, uint16_t FrameSize);

/**
 *  @brief This API shall terminate the Linux Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_ServerDisconnect(uint32_t *ServerSocket);


#endif /* SERVER_H */
