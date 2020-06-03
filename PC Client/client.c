/**
 *  @file client.c
 *  @author May Alaa
 *  @brief PC client interfaces
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "Frame.h"

typedef struct
{
	uint8_t* Message;
	uint8_t MessageSize;
}MessageInfo_t;
#define NUM_OF_MSGS			5
#define IS_MSG_VALID(MSG)	((MSG == MESSAGE_ACK)				|| \
						    (MSG == MESSAGE_NACK)   			|| \
                            (MSG == MESSAGE_HEADER_FRAME)   	|| \
                            (MSG == MESSAGE_DATA_FRAME)   		|| \
                            (MSG == MESSAGE_CONNECTION_KEY))



static void recv_print(uint32_t size);
static void bin(unsigned n);

/* CLIENT GLOBAL VARIABLES */

struct sockaddr_in servaddr;
uint32_t slen = sizeof(servaddr);
uint32_t ClientSocket = 0;
uint32_t ConnectionKey = CONNECTION_KEY;
uint16_t Iterator = 0;
uint8_t Status = NACK;

uint8_t recvBuf[512] = {0};

/////////////////////////// FRAME GLOBALS //////////////////////////////
uint8_t *Frame = NULL;

uint8_t TxFrameDataBuffer[512];
uint8_t RxFrameDataBuffer[512];

FrameData_t *TxFrameData = (FrameData_t *)TxFrameDataBuffer;
FrameData_t *RxFrameData = (FrameData_t *)RxFrameDataBuffer;

FrameHeader_t TxFrameHeader = 
{
	.Signature 		= 	SIGNATURE,	
	.NumOfCommands	= 	NUM_OF_PERIPH,
	.TotalDataSize	= 	0
};

FrameHeader_t RxFrameHeader;

uint32_t FrameTotalSize = 0;
uint32_t DIO_DATA_SIZE = 0;
uint32_t PWM_DATA_SIZE = 0;


//>>PROBLEM IN ACK & NACK
// MessageInfo_t MessageInfo[NUM_OF_MSGS] = {
	// {.Message = , .MessageSize = },
	// {.Message = , .MessageSize = },
	// {.Message = , .MessageSize = },
	// {.Message = , .MessageSize = },
	// {.Message = , .MessageSize = }
	
// };

//////////////////////////////////////////////////////////////////////////

uint8_t UDP_ClientConnect(uint8_t* ServerIP, uint16_t ServerPort)
{
	uint8_t TimeoutCounter = 0;
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
	servaddr.sin_port = htons(ServerPort);
	servaddr.sin_addr.S_un.S_addr = inet_addr(ServerIP); 
	
	
	/*********** CONNECTION KEY VERIFICATION ***********/
	UDP_ClientSend(MESSAGE_CONNECTION_KEY);
	
	while(TimeoutCounter < 5)
	{
		printf("Trial: %d\n", TimeoutCounter);
		if(UDP_ClientReceive(MESSAGE_ACK) == MESSAGE_ACK)
		{
			printf("CONNECTION KEY ACCEPTED\n");
			TimeoutCounter = 0;
			return CONNECTION_OK;
		}
		else
		{
			printf("Connection Trial[%d] failed, Retrying......\n", TimeoutCounter);
			TimeoutCounter++;
			// if(TimeoutCounter == 4) 
				// ConnectionKey = CONNECTION_KEY;
			
			UDP_ClientSend(MESSAGE_CONNECTION_KEY);
		}
	}
	return CONENCTION_REQUEST_TIMEOUT;
}

void UDP_ClientSend(uint8_t MessageType)
{
	// if(IS_MSG_VALID(MessageType))
	// {
		
		
	// }
	// else
	// {
		// printf("MESSAGE_TYPE_ERROR\n");	
	// }	
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
			if (sendto(ClientSocket, (uint8_t *)&TxFrameHeader, sizeof(FrameHeader_t), 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
		case MESSAGE_DATA_FRAME:	
			if (sendto(ClientSocket, (uint8_t *)Frame, TxFrameHeader.TotalDataSize, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
			
		case MESSAGE_CONNECTION_KEY:	
			if (sendto(ClientSocket, (uint8_t *)&ConnectionKey, sizeof(uint32_t), 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
			
		default:
			
			break;
	}
}

uint8_t UDP_ClientReceive(uint8_t MessageType)
{
	uint8_t returnType = 0;
	//clear the buffer by filling null, it might have previously received data
	
	switch(MessageType)
	{
		case MESSAGE_ACK:
		memset(RxFrameDataBuffer, 0, STATUS_SIZE);
		if (recvfrom(ClientSocket, (uint8_t *)RxFrameDataBuffer, STATUS_SIZE, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}		
		recv_print(STATUS_SIZE);
		if(RxFrameDataBuffer[0] == ACK)
		{
			returnType = MESSAGE_ACK;	
		}
		else
		{
			returnType = MESSAGE_NACK;
		}
		break;

		case MESSAGE_HEADER_FRAME:
			memset(&RxFrameHeader, 0, sizeof(FrameHeader_t));
			if (recvfrom(ClientSocket, (uint8_t*)&RxFrameHeader, sizeof(FrameHeader_t), 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			recv_print(sizeof(FrameHeader_t));
			
			if(RxFrameHeader.Signature == SIGNATURE)
			{
				returnType = HEADER_VALID;
			}
			else
			{
				returnType = HEADER_INVALID;
			}
			break;
			
		case MESSAGE_DATA_FRAME:
			memset(RxFrameDataBuffer, 0, RxFrameHeader.TotalDataSize);
			if (recvfrom(ClientSocket, RxFrameDataBuffer, RxFrameHeader.TotalDataSize, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}	
			recv_print(RxFrameHeader.TotalDataSize);
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
void FRAME_GenerateDataFrame(uint8_t* DIO_Data, uint32_t DIO_DataSize , uint8_t* PWM_Data, uint32_t PWM_DataSize)
{
	uint8_t PeripheralIndex = 0;
	DIO_DATA_SIZE = DIO_DataSize;
	PWM_DATA_SIZE = PWM_DataSize;
	
	/* Grouping for easier indexing */
	uint32_t local_PeripheralID[NUM_OF_PERIPH] = {DIO_PERIPHERAL_ID, PWM_PERIPHERAL_ID};
	uint32_t local_PeripheralDataSize[NUM_OF_PERIPH] = {DIO_DataSize, PWM_DataSize};
	uint8_t *local_PeripheralData[NUM_OF_PERIPH] = {DIO_Data, PWM_Data};
	
	/* Alloctating the size of the frame to be sent */
	FrameTotalSize = DIO_DataSize + PWM_DataSize + PERIPH_INFO_SIZE;
	TxFrameHeader.TotalDataSize = FrameTotalSize;
	
	Frame = (uint8_t *) calloc(FrameTotalSize, sizeof(uint8_t));
	/* Pointer to hold the current position while transitioning in the frame */	
	uint8_t *Pointer = (uint8_t *)Frame;
	
	for(PeripheralIndex = 0; PeripheralIndex < NUM_OF_PERIPH; PeripheralIndex++)
	{
		/* Copying the Peripheral ID */
		memcpy(Pointer, &local_PeripheralID[PeripheralIndex], sizeof(uint32_t));
		Pointer += sizeof(uint32_t); /* Moving the pointer by "PeripheralID" size */
		/* Copying the DataSize */
		memcpy(Pointer, &local_PeripheralDataSize[PeripheralIndex], sizeof(uint32_t));
		Pointer += sizeof(uint32_t); /* Moving the pointer by "DataSize" size */
		/* Copying the Data */
		memcpy(Pointer, local_PeripheralData[PeripheralIndex], local_PeripheralDataSize[PeripheralIndex]);
		Pointer += local_PeripheralDataSize[PeripheralIndex]; /* Moving the pointer by to point on the next Peripheral */
	}
}

void FRAME_FreeData(void)
{
	free(Frame);
}

uint8_t FRAME_ParsingDataFrame(void)
{
	uint8_t PeripheralIndex = 0;
	
	/* Arrays holding the Readings to be passed to the GUI */
	uint8_t DIO_Readings[DIO_INPUT_PINS] = {0};
	uint8_t PWM_Readings[512] = {0}; //can be allocated by calloc later
	
	/*Grouping for easier indexing */
	uint8_t* Rx_Readings[NUM_OF_PERIPH] = {DIO_Readings, PWM_Readings};
	
	uint8_t *RxFrameData = (uint8_t *)RxFrameDataBuffer;
	
	for(PeripheralIndex = 0; PeripheralIndex < NUM_OF_PERIPH; PeripheralIndex++)
	{
		memcpy(Rx_Readings[PeripheralIndex], &(((FrameData_t *)RxFrameData)->PeripheralData), ((FrameData_t *)RxFrameData)->DataSize);
		RxFrameData += (PERIPH_HEADER_SIZE + ((FrameData_t *)RxFrameData)->DataSize);
	}
	
	uint8_t Iterator = 0;
	for(Iterator = 0; Iterator < DIO_DATA_SIZE; Iterator++)
	{
		printf("DIO_READING[%d]: %d\n", Iterator, DIO_Readings[Iterator]);
	}
	
	for(Iterator = 0; Iterator < PWM_DATA_SIZE; Iterator++)
	{
		printf("PWM_READING[%d]: %d\n", Iterator, PWM_Readings[Iterator]);
	}
	
	///////////////////// CONVERTING TO INT /////////////////////
	uint8_t DIO_BitValue = 0;
	for(Iterator = 0; Iterator < DIO_DATA_SIZE; Iterator++)
	{
		DIO_BitValue |= (DIO_Readings[Iterator]<<Iterator);
	}
	printf("\n\nDIO_BitValue: ");
	bin(DIO_BitValue);
	
	printf("\n\nDIO_RETURNNNN:%d", DIO_BitValue);
	return DIO_BitValue;
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
static void recv_print(uint32_t size)
{
	uint32_t i = 0;
	for(i = 0; i < size; i++)
	{
		printf("Byte[%d]: %d\n", i, RxFrameDataBuffer[i]);
	}
	printf("\n");
}

static void bin(unsigned n) 
{ 
    /* step 1 */
    if (n > 1) 
        bin(n/2); 
  
    /* step 2 */
    printf("%d", n % 2); 
}


