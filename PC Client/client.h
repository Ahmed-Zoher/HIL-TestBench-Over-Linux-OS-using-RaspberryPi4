#ifndef CLIENT_H
#define CLIENT_H


#include <winsock2.h>
#pragma comment(lib,"ws2_32") 	//Winsock Library

/************* MACROS DEFINITIONS ************/
#define SERVER 				"192.168.5.10"	//ip address of udp server
#define BUFLEN 				512				//Max length of buffer
#define PORT 				8080			//The port on which to listen for incoming data
#define ACK_SIZE 			4
#define NACK_SIZE 			5
#define NUM_OF_FRAMES		3

/********** PC CLIENT INTERFACES ***********/

/**
 *  @brief This API shall initialize the PC Client UDP link.
 *  
 *  @param [in] si_other Socket
 *  @return void
 */
void UDP_ClientInit(uint32_t *ClientSocket, struct sockaddr_in *si_other);

/**
 *  @brief This API shall send a frame
 *  
 *  @param [in] buffer   Buffer 
 *  @param [in] servaddr Server addresss
 *  @param [in] len      Length of frame being sent
 *  @return void
 */
void UDP_ClientSend(uint32_t *ClientSocket, uint8_t* buffer, struct sockaddr_in *servaddr, uint32_t len, uint32_t FrameSize);

/**
 *  @brief This API shall receive and acknowledgment.
 *  
 *  @param [in] servaddr Server address
 *  @param [in] len      Length of frame being received
 *  @return void
 */
void UDP_ClientReceive(uint32_t *ClientSocket, uint8_t* buffer, struct sockaddr_in *servaddr, uint32_t * len, uint32_t FrameSize);

/**
 *  @brief This API shall terminate the PC Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_ClientDisconnect(uint32_t *ClientSocket);


#endif /* CLIENT_H */