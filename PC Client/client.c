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


//uint8_t buf[BUFLEN];
uint8_t recvBuf[STATUS_SIZE];
uint32_t ClientSocket;

uint8_t RxFrameHeaderBuffer[100];
uint8_t RxFrameDataBuffer[100];

struct sockaddr_in servaddr;
uint32_t slen = sizeof(servaddr);
uint16_t Iterator = 0;

uint8_t Status = NACK;

uint8_t ConnectionStatus = CONNECTION_OK;

/**** Frame variables **/
uint8_t *Frame = NULL;
uint32_t FrameTotalSize = 0;

typedef struct
{
	uint8_t MessageType;
	uint8_t MessageSize;
}MessageInfo;

/////////////////////////// FRAME GLOBALS //////////////////////////////
uint8_t DIO_Data[DIO_DATA_SIZE] = {45, 120, 78, 42, 11};
uint8_t UZART_Data[UZART_DATA_SIZE] = {23, 10, 117};


FrameHeader_t FrameHeader = 
{
	.Signature 		= 	SIGNATURE,	
	.NumOfCommands	= 	NUM_OF_CMD,
	.TotalDataSize	= 	0
};

//////////////////////////////////////////////////////////////////////////


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

void UDP_ClientSend(uint8_t MessageType)
{
	switch(MessageType)
	{
		case MESSAGE_ACK:
			Status = ACK;
			if (sendto(ClientSocket, (uint8_t *)&Status, STATUS_SIZE, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;

		case MESSAGE_NACK:
			Status = NACK;
			if (sendto(ClientSocket, (uint8_t *)&Status, STATUS_SIZE, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
		case MESSAGE_HEADER_FRAME:
			if (sendto(ClientSocket, (uint8_t *)&FrameHeader, sizeof(FrameHeader_t), 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
		case MESSAGE_DATA_FRAME:	
			if (sendto(ClientSocket, (uint8_t *)Frame, FrameHeader.TotalDataSize, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
			
		default:
			printf("MESSAGE_TYPE_ERROR\n");
			break;
	}
}

uint8_t UDP_ClientReceive(uint8_t MessageType)
{
	uint8_t returnType = 0;
	//clear the buffer by filling null, it might have previously received data
	memset(recvBuf, 0, BUFLEN);
	
	switch(MessageType)
	{
		case MESSAGE_ACK:
		//try to receive some data, this is a blocking call
		if (recvfrom(ClientSocket, (uint8_t *)recvBuf, STATUS_SIZE, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
		{
			printf("USELESS PRINT 2\n");
			printf("recvfrom() failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("%s\n", recvBuf);
		if(strcmp(recvBuf, "ACK") == 0)
		{
			returnType = MESSAGE_ACK;	
		}
		else
		{
			returnType = MESSAGE_NACK;
		}
		break;

		case MESSAGE_HEADER_FRAME:
			printf("FML1\n");
			if (recvfrom(ClientSocket, recvBuf, sizeof(FrameHeader_t), 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			printf("%s\n",recvBuf);
			returnType = MESSAGE_HEADER_FRAME;
			break;
			
		case MESSAGE_DATA_FRAME:
		
			printf("FML2\n");
			if (recvfrom(ClientSocket, recvBuf, FrameHeader.TotalDataSize, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			printf("%s\n",recvBuf);
			returnType = MESSAGE_DATA_FRAME;
			break;
			
		default:
			printf("MESSAGE_TYPE_ERROR\n");
			break;
	}
	
	return returnType;
}

void UDP_ClientDisconnect(void)
{
	closesocket(ClientSocket);
	WSACleanup();
}

////////////////////////////////////////////FRAME APIS////////////////////////////////////////
void FRAME_Generate(uint8_t* DIO_Data, uint32_t DIO_DataSize , uint8_t* UART_Data, uint32_t UART_DataSize)
{
	uint16_t CMDIndex = 0;
	uint16_t DataIndex = 0;
	uint16_t FrameIndex = 0;
	
	FrameData_t FrameData[NUM_OF_CMD] = 
	{
		{
			.PeripheralID	= 	DIO_PERIPHERAL_ID,	
			.DataSize		= 	DIO_DataSize,		
			.PeripheralData	= 	DIO_Data
		},

		{
			.PeripheralID	= 	UZART_PERIPHERAL_ID,	
			.DataSize		= 	UART_DataSize,		
			.PeripheralData	= 	UART_Data
		}
	};
	
	FrameTotalSize = DIO_DataSize + UART_DataSize + (NUM_OF_CMD * PERIPH_HEADER_SIZE);
	FrameHeader.TotalDataSize = FrameTotalSize;
	Frame = (uint8_t *) calloc(FrameTotalSize, sizeof(uint8_t));
	
	for(CMDIndex = 0; CMDIndex < NUM_OF_CMD; CMDIndex++)
	{
		Frame[FrameIndex] = FrameData[CMDIndex].PeripheralID; //2 bytes
		FrameIndex += 2;
		Frame[FrameIndex] = FrameData[CMDIndex].DataSize; //2 bytes
		FrameIndex += 2;
		
		for(DataIndex = 0; DataIndex < FrameData[CMDIndex].DataSize; DataIndex++)
		{
			Frame[FrameIndex] = FrameData[CMDIndex].PeripheralData[DataIndex];
			FrameIndex++;
		}
	}
}

void FRAME_Print(void)
{
	uint16_t Iterator = 0;
	for(Iterator = 0; Iterator < FrameTotalSize; Iterator++)
	{
		printf("Frame-Byte[%d]: %d\n", Iterator, Frame[Iterator]);
	}
}


//////////////////////////////////////////////////////////////////////////
