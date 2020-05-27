#ifndef CLIENT_H
#define CLIENT_H

/************* MACROS DEFINITIONS ************/
#define SERVER 				"192.168.5.10"	//ip address of udp server
#define BUFLEN 				512				//Max length of buffer
#define PORT 				8080				//The port on which to listen for incoming data
#define ACK_SIZE 			4
#define NUM_OF_FRAMES		3

typedef struct{
	uint32_t ID;
	uint32_t CMD;
	uint8_t Ay7aga;
}Test_frame;

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
void UDP_ClientSendFrame(uint32_t *ClientSocket, uint8_t * buffer, struct sockaddr_in *servaddr, uint32_t len);

/**
 *  @brief This API shall receive and acknowledgment.
 *  
 *  @param [in] servaddr Server address
 *  @param [in] len      Length of frame being received
 *  @return void
 */
void UDP_ClientReceiveACK(uint32_t *ClientSocket, struct sockaddr_in *servaddr, uint32_t * len);

/**
 *  @brief This API shall terminate the PC Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_ClientDisconnect(uint32_t *ClientSocket);


#endif /* CLIENT_H */