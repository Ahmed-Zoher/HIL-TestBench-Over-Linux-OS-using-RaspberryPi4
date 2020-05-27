/**
 *  @file client.c
 *  @author May Alaa
 *  @brief PC client interfaces
 */

#include <stdio.h>
#include <stdint.h>
#include "client.h"



/**
 *  @brief This API shall initialize the PC Client UDP link.
 *  
 *  @param [in] si_other Socket
 *  @return void
 */
void UDP_ClientInit(uint32_t *ClientSocket, struct sockaddr_in *si_other)
{
	WSADATA wsa;
	//Initialise winsock
	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Initialization Failed. Error Code : %d\n",WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");
	
	//create socket
	if ((*ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() Failed. Error Code : %d\n" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	//setup address structure
	memset((char *) si_other, 0, sizeof(struct sockaddr_in ));
	si_other->sin_family = AF_INET;
	si_other->sin_port = htons(PORT);
	si_other->sin_addr.S_un.S_addr = inet_addr(SERVER);
}

/**
 *  @brief This API shall send a frame
 *  
 *  @param [in] buffer   Buffer 
 *  @param [in] servaddr Server addresss
 *  @param [in] len      Length of frame being sent
 *  @return void
 */
void UDP_ClientSend(uint32_t *ClientSocket, uint8_t* buffer, struct sockaddr_in *servaddr, uint32_t len, uint32_t FrameSize)
{
	//send the message
	if (sendto(*ClientSocket, buffer, FrameSize, 0, (struct sockaddr *)servaddr, len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}


void UDP_ClientReceive(uint32_t *ClientSocket, uint8_t* buffer, struct sockaddr_in *servaddr, uint32_t * len, uint32_t FrameSize)
{
	//clear the buffer by filling null, it might have previously received data
	memset(buffer,0, BUFLEN);
	//try to receive some data, this is a blocking call
	if (recvfrom(*ClientSocket, buffer, FrameSize, 0, (struct sockaddr *)servaddr, len) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("%s\n",buffer);
}

/**
 *  @brief This API shall terminate the PC Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_ClientDisconnect(uint32_t *ClientSocket)
{
	closesocket(*ClientSocket);
	WSACleanup();
}

