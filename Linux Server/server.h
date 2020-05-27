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
#define NACK_SIZE		5
#define NUM_OF_FRAMES	3

/************* TYPES DEFINITIONS ************/
typedef struct
{
	uint32_t Signature;
	uint16_t NumOfCommands;
	uint16_t TotalDataSize;
}FrameHeader_t;

typedef struct
{
	uint32_t PeripheralID;
	uint32_t DataSize;
	uint8_t* PeripheralData;
}FrameData_t;

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
