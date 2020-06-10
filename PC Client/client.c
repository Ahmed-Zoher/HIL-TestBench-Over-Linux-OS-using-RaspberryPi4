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
#define NUM_OF_MSGS				5
#define IS_MSG_VALID(MSG)		((MSG == MESSAGE_ACK)				|| \
								(MSG == MESSAGE_NACK)   			|| \
								(MSG == MESSAGE_HEADER_FRAME)   	|| \
								(MSG == MESSAGE_DATA_FRAME)   		|| \
								(MSG == MESSAGE_CONNECTION_KEY))

#define DATA_FRAME				0
#define SERIAL_FRAME			1
#define SERIAL_RETURN_FRAME		2
#define READINGS_FRAME			3


static void recv_print(uint32_t size);

/* CLIENT GLOBAL VARIABLES */

struct sockaddr_in servaddr;
uint32_t slen = sizeof(servaddr);
uint32_t ClientSocket = 0;
uint32_t ConnectionKey = CONNECTION_KEY;
uint16_t Iterator = 0;
uint8_t Status = NACK;

uint8_t recvBuf[512] = {0};

/////////////////////////// FRAME GLOBALS //////////////////////////////

/* Tx Data Frame */
uint8_t *Frame = NULL;
uint32_t FrameTotalSize = 0;
//uint8_t TxFrameDataBuffer[512];

/* Tx Serial Frame */
uint8_t *SerialFrame = NULL;

uint8_t *UART_Frame = NULL;
uint32_t UART_FrameSize = 0;

uint8_t *SPI_CH1_Frame = NULL;
uint32_t SPI_CH1_FrameSize = 0;

uint8_t *SPI_CH2_Frame = NULL;
uint32_t SPI_CH2_FrameSize = 0;

uint32_t SerialSize = 0;
uint8_t* SerialBuffer = NULL;

/* A buffer holding the Readings to be passed to the GUI */
uint32_t *Rx_ReadingsFrame = NULL;

/* Rx Data Frame */
uint8_t RxFrameDataBuffer[512];

//FrameData_t *TxFrameData = (FrameData_t *)TxFrameDataBuffer;
FrameData_t *RxFrameData = (FrameData_t *)RxFrameDataBuffer;

FrameHeader_t TxFrameHeader = 
{
	.Signature 		= 	SIGNATURE,	
	.NumOfCommands	= 	NUM_OF_PERIPH,
	.TotalDataSize	= 	0
};

FrameHeader_t RxFrameHeader;

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
			
		case MESSAGE_UART:
			if(UART_FrameSize > 0)
			{
				if (sendto(ClientSocket, (uint8_t *)UART_Frame, UART_FrameSize, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code : %d" , WSAGetLastError());
					exit(EXIT_FAILURE);
				}
			}
			break;
			
		case MESSAGE_SPI_CH1:	
			if (sendto(ClientSocket, (uint8_t *)SPI_CH1_Frame, SPI_CH1_FrameSize, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			break;
			
		case MESSAGE_SPI_CH2:	
			if (sendto(ClientSocket, (uint8_t *)SPI_CH2_Frame, SPI_CH2_FrameSize, 0, (struct sockaddr *)&servaddr, slen) == SOCKET_ERROR)
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
			printf("HERE1\n");
			printf("recvfrom() failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}		
		printf("ACKNOWLEDGMENT_STATUS: %d\n", RxFrameDataBuffer[0]);
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
				printf("HERE2\n");
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}			
			if(RxFrameHeader.Signature == SIGNATURE)
			{
				returnType = HEADER_VALID;
			}
			else
			{
				returnType = HEADER_INVALID;
			}
			for(Iterator = 0; Iterator < sizeof(FrameHeader_t); Iterator++)
			{
				printf("RX_HEADER_FRAME_BYTE[%d]: %d\n", Iterator, ((uint8_t*)&RxFrameHeader)[Iterator]);
			}
			printf("\n");
			break;
			
		case MESSAGE_DATA_FRAME:
			memset(RxFrameDataBuffer, 0, RxFrameHeader.TotalDataSize);
			if (recvfrom(ClientSocket, RxFrameDataBuffer, RxFrameHeader.TotalDataSize, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
			{
				printf("HERE3\n");
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			for(Iterator = 0; Iterator < RxFrameHeader.TotalDataSize; Iterator++)
			{
				printf("RX_DATA_FRAME_BYTE[%d]: %d\n", Iterator, RxFrameDataBuffer[Iterator]);
			}
			printf("\n");
			returnType = MESSAGE_DATA_FRAME;
			break;
	
		///////////////////////////CHANGES HERE		
		case MESSAGE_SERIAL_SIZE:
			if (recvfrom(ClientSocket, (uint8_t *)&SerialSize, sizeof(uint32_t), 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
			{
				printf("HERE4\n");
				printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			printf("SerialSize: %d\n", SerialSize);
			if(SerialSize != 0)
			{
				returnType = MESSAGE_SERIAL_SIZE;
			}
			else
			{
				returnType = MESSAGE_NACK;
			}
			break;
		
		case MESSAGE_UART:
			if(SerialSize > 0)
			{
				memset(RxFrameDataBuffer, 0, SerialSize);
				if (recvfrom(ClientSocket, RxFrameDataBuffer, SerialSize, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
				{
					printf("HERE5\n");
					printf("recvfrom() failed with error code : %d" , WSAGetLastError());
					exit(EXIT_FAILURE);
				}
				for(Iterator = 0; Iterator < SerialSize; Iterator++)
				{
					printf("RX_UART_MESSAGE_BYTE[%d]: %d\n", Iterator, RxFrameDataBuffer[Iterator]);
				}
			}
			returnType = MESSAGE_DATA_FRAME;
			break;
			
		case MESSAGE_SPI_CH1:
			if(SerialSize > 0)
			{
				memset(RxFrameDataBuffer, 0, SerialSize);
				if (recvfrom(ClientSocket, RxFrameDataBuffer, SerialSize, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
				{
					printf("HERE6\n");
					printf("recvfrom() failed with error code : %d" , WSAGetLastError());
					exit(EXIT_FAILURE);
				}	
				recv_print(SerialSize);
			}
			returnType = MESSAGE_SPI_CH1;
			break;
			
		case MESSAGE_SPI_CH2:
			if(SerialSize > 0)
			{
				memset(RxFrameDataBuffer, 0, SerialSize);
				if (recvfrom(ClientSocket, RxFrameDataBuffer, SerialSize, 0, (struct sockaddr *)&servaddr, &slen) == SOCKET_ERROR)
				{
					printf("HERE7\n");
					printf("recvfrom() failed with error code : %d" , WSAGetLastError());
					exit(EXIT_FAILURE);
				}	
				recv_print(SerialSize);
			}
			returnType = MESSAGE_SPI_CH2;
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
void FRAME_GenerateDataFrame(uint8_t* DIO_Data, uint8_t* PWM_Config, uint8_t* UART_Config, uint8_t* SPI_CH1_Config, uint8_t* SPI_CH2_Config)
{
	uint8_t PeripheralIndex = 0;
	
	// DIO_ID .. DIO_DATA_SIZE .. DIO_DATA .. PWM_ID .. PWM_DATA_SIZE .. PWM_DATA ... UART_ID .. UART_DATA_SIZE(1) .. UART_DATA(state)
	
	/* Grouping for easier indexing */
	uint32_t local_PeripheralID[NUM_OF_PERIPH] = {DIO_PERIPHERAL_ID, PWM_PERIPHERAL_ID, UART_PERIPHERAL_ID, SPI_CH1_PERIPHERAL_ID, SPI_CH2_PERIPHERAL_ID};
	uint32_t local_PeripheralDataSize[NUM_OF_PERIPH] = {DIO_INPUT_PINS, PWM_CONFIG_SIZE, UART_CONFIG_SIZE, SPI_CH1_CONFIG_SIZE, SPI_CH2_CONFIG_SIZE};
	uint8_t *local_PeripheralData[NUM_OF_PERIPH] = {DIO_Data, PWM_Config, UART_Config, SPI_CH1_Config, SPI_CH2_Config};
	
	/* Alloctating the size of the frame to be sent */
	FrameTotalSize = 0;
	for(PeripheralIndex = 0; PeripheralIndex < NUM_OF_PERIPH; PeripheralIndex++)
		FrameTotalSize += local_PeripheralDataSize[PeripheralIndex];
	
	FrameTotalSize += PERIPH_INFO_SIZE;
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

void FRAME_GenerateSerialFrame(uint8_t *Serial_Data, uint32_t Serial_DataSize, uint8_t SerialIndex)
{
	uint32_t local_PeripheralID = 0;
	
	SerialFrame = (uint8_t *)calloc((PERIPH_ID_SIZE + Serial_DataSize), sizeof(uint8_t));
	memcpy((SerialFrame + PERIPH_ID_SIZE), Serial_Data, Serial_DataSize);
	
	switch(SerialIndex)
	{
		case SERIAL_UART:
			local_PeripheralID = UART_PERIPHERAL_ID;
			UART_FrameSize = Serial_DataSize;
			memcpy(SerialFrame, &local_PeripheralID, PERIPH_ID_SIZE);
			UART_Frame = SerialFrame;
			break;
		case SERIAL_SPI_CH1:
			local_PeripheralID = SPI_CH1_PERIPHERAL_ID;
			SPI_CH1_FrameSize = Serial_DataSize;
			memcpy(SerialFrame, &local_PeripheralID, PERIPH_ID_SIZE);
			SPI_CH1_Frame = SerialFrame;
			break;		
		case SERIAL_SPI_CH2:
			local_PeripheralID = SPI_CH2_PERIPHERAL_ID; 
			SPI_CH2_FrameSize = Serial_DataSize;
			memcpy(SerialFrame, &local_PeripheralID, PERIPH_ID_SIZE);
			SPI_CH2_Frame = SerialFrame;
			break;
		default:
			printf("Physiiick .. U GOT THE RONG NUMBAH\n");
			break;
	}
	
	printf("\n", Iterator);
	for(Iterator = 0; Iterator < Serial_DataSize; Iterator++)
	{
		printf("Serial_Frame[%d]: %02X\n", Iterator, SerialFrame[Iterator]);
	}
}

uint8_t *FRAME_SerialReturnFrame(void)
{
	SerialBuffer = (uint8_t *)calloc(SerialSize + 1, sizeof(uint8_t));

	for (Iterator = 0; Iterator < SerialSize; Iterator++)
	{
		printf("RX_FRAME_BUFFER_UART_BYTE[%d]: %02X\n", Iterator, RxFrameDataBuffer[Iterator]);
		if (RxFrameDataBuffer[Iterator] < 16)
		{
			sprintf(SerialBuffer + Iterator, "%01X", RxFrameDataBuffer[Iterator]);
		}
		else
		{
			sprintf(SerialBuffer + Iterator * 2, "%02X", RxFrameDataBuffer[Iterator]);
		}    
	}
	printf("BUFFER: %s\n", SerialBuffer);
	SerialSize = 0;
	return SerialBuffer;
}

uint32_t *FRAME_ReadingsFrame(void)
{
	uint32_t Rx_TotalDataSize = 0;
	
	/* A pointer to navigate through the Rx Buffer */
	uint8_t *TransitionPointer = NULL;
	
	/* A pointer to navigate through the Readings Buffer */
	uint8_t *ReadingPointer = NULL;

	TransitionPointer = (uint8_t *)RxFrameDataBuffer;

	/* Scanning for sizes */
	for(Iterator = 0; Iterator < NUM_RX_PERIPH; Iterator++)
	{
		Rx_TotalDataSize += ((FrameData_t *)TransitionPointer)->DataSize;
		TransitionPointer  += (PERIPH_HEADER_SIZE + ((FrameData_t *)TransitionPointer)->DataSize);
	}
	
	Rx_ReadingsFrame = (uint32_t *)calloc((Rx_TotalDataSize/sizeof(uint32_t)), sizeof(uint32_t));
	
	ReadingPointer = (uint8_t *)Rx_ReadingsFrame;
	
	/* Returning the pointer to the initial position of the Rx buffer */
	TransitionPointer = (uint8_t *)RxFrameDataBuffer;
	
	for(Iterator = 0; Iterator < NUM_RX_PERIPH; Iterator++)
	{
		memcpy(ReadingPointer, &(((FrameData_t *)TransitionPointer)->PeripheralData), ((FrameData_t *)TransitionPointer)->DataSize);
		TransitionPointer += (PERIPH_HEADER_SIZE + ((FrameData_t *)TransitionPointer)->DataSize);
		ReadingPointer += ((FrameData_t *)TransitionPointer)->DataSize;
	}
	
	return Rx_ReadingsFrame;
}

void FRAME_Print(void)
{
	for(Iterator = 0; Iterator < FrameTotalSize; Iterator++)
	{
		printf("Frame-Byte[%d]: %d\n", Iterator, Frame[Iterator]);
	}
}

//////////////////////////////////////////////////////////////////////////
static void recv_print(uint32_t size)
{
	uint32_t Iterator = 0;
	for(Iterator = 0; Iterator < size; Iterator++)
	{
		printf("Byte[%d]: %d\n", Iterator, RxFrameDataBuffer[Iterator]);
	}
	printf("\n");
}

void FRAME_FreeBuffer(uint8_t Buffer)
{
	switch(Buffer)
	{
		case DATA_FRAME				:	free(Frame);			break;
		case SERIAL_FRAME			:	free(SerialFrame);		break;
		case SERIAL_RETURN_FRAME	:	free(SerialBuffer);		break;
		case READINGS_FRAME			:	free(Rx_ReadingsFrame);	break;
	}
}

////////////////////////////////////////////////////////////////////////////////
// void FRAME_FreeData(void)
// {
	// free(Frame);
// }

// void FRAME_ReturnSerialFree(void)
// {
	// free(SerialBuffer);
// }
// void FRAME_FreeReadingsBuffer(void)
// {
	// free(Rx_ReadingsFrame);	
// }

////////////////////////////////////////////////////////////////////////////////



// uint8_t FRAME_ParsingDataFrame(void)
// {
	// uint8_t PeripheralIndex = 0;
	// /* Arrays holding the Readings to be passed to the GUI */
	// uint8_t DIO_Readings[DIO_INPUT_PINS] = {0};
	// uint8_t PWM_Readings[20] = {0};
	//CHANGES IN PWM READINGS to 20
	
	// /*Grouping for easier indexing */
	// uint8_t* Rx_Readings[NUM_OF_PERIPH] = {DIO_Readings, PWM_Readings};
	
	// uint8_t *RxFrameData = (uint8_t *)RxFrameDataBuffer;
	// printf("FAR1\n");
	
	//FORCED TO GET DIO AND PWM ONLY  NUM_OF_PERIPH>>2 
	// printf("HABAL FEL GBAL: %d", PeripheralIndex);
	// for(PeripheralIndex = 0; PeripheralIndex < 2; PeripheralIndex++)
	// {
		// printf("STEP1\n");
		// memcpy(Rx_Readings[PeripheralIndex], &(((FrameData_t *)RxFrameData)->PeripheralData), ((FrameData_t *)RxFrameData)->DataSize);
		// printf("STEP2\n");
		// printf("REFAAAAAT'ssssss >>>> BYTE: %d", ((FrameData_t *)RxFrameData)->DataSize);
		// RxFrameData += (PERIPH_HEADER_SIZE + ((FrameData_t *)RxFrameData)->DataSize);
		// printf("STEP3\n");
	// }
	// printf("FAR2\n");
	
	// for(Iterator = 0; Iterator < DIO_INPUT_PINS; Iterator++)
	// {
		// printf("DIO_READING[%d]: %d\n", Iterator, DIO_Readings[Iterator]);
	// }
	
	// for(Iterator = 0; Iterator < PWM_CONFIG_SIZE; Iterator++)
	// {
		// printf("PWM_READING[%d]: %d\n", Iterator, PWM_Readings[Iterator]);
	// }
	// printf("FAR3\n");
	/////////////////// CONVERTING TO INT /////////////////////
	// uint8_t DIO_BitValue = 0;
	// for(Iterator = 0; Iterator < DIO_INPUT_PINS; Iterator++)
	// {
		// DIO_BitValue |= (DIO_Readings[Iterator]<<Iterator);
	// }
	// printf("\n\nDIO_BitValue: ");
	// bin(DIO_BitValue);
	
	// printf("\n\nDIO_RETURNNNN:%d", DIO_BitValue);
	// printf("FAR4\n");
	// return DIO_BitValue;
// }

