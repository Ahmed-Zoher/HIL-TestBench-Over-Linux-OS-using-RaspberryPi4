/**
 *  @file client.c
 *  @author May Alaa
 *  @brief PC client interfaces
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "client.h"
#include "Frame.h"


uint8_t buf[BUFLEN];
uint8_t recvBuf[ACK_SIZE];
uint32_t ClientSocket;

uint8_t RxFrameHeaderBuffer[100];
uint8_t RxFrameDataBuffer[100];

struct sockaddr_in servaddr;
uint32_t slen = sizeof(servaddr);
uint16_t Iterator = 0;
uint8_t *Frame = NULL;
uint8_t ACK_arr[] = "ACK";
uint8_t NACK_arr[] = "NACK";

uint8_t ConnectionStatus = CONNECTION_OK;

FrameHeader_t FrameHeader = 
{
	.Signature 		=  SIGNATURE,
	.NumOfCommands 	=  NUM_OF_CMD,
	.TotalDataSize 	=  TOTAL_SIZE
};

uint8_t UDP_ClientConnect(void)
{
	WSADATA wsa;
	//Initialise winsock
	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Initialization Failed. Error Code : %d\n",WSAGetLastError());
		//exit(EXIT_FAILURE);
		return CONNECTION_WINSOCK_INIT_ERROR;
	}
	printf("Initialised.\n");
	
	//create socket
	if ((ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() Failed. Error Code : %d\n" , WSAGetLastError());
		//exit(EXIT_FAILURE);
		return CONNECTION_SOCKET_ERROR;
	}
		
	//setup address structure
	memset((char *) &servaddr, 0, sizeof(struct sockaddr_in ));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.S_un.S_addr = inet_addr(SERVER);
	
	return CONNECTION_OK;
}


void UDP_ClientSend(uint8_t* buffer, uint32_t FrameSize)
{
	//send the message
	if (sendto(ClientSocket, buffer, FrameSize, 0, (struct sockaddr *)servaddr, len) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}


void UDP_ClientReceive(uint8_t* buffer, uint32_t FrameSize)
{
	//clear the buffer by filling null, it might have previously received data
	memset(buffer,0, BUFLEN);
	//try to receive some data, this is a blocking call
	if (recvfrom(ClientSocket, buffer, FrameSize, 0, (struct sockaddr *)servaddr, &len) == SOCKET_ERROR)
	{
		printf("recvfrom() failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("%s\n",buffer);
}


void UDP_ClientDisconnect(void)
{
	closesocket(ClientSocket);
	WSACleanup();
}

