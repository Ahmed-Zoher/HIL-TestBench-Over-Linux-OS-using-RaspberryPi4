/**
 *  @file Application.c
 *  @author Hazem Mekawy
 *  @brief Linux Server Application
 */

#include "server.h"
#include "Frame.h"

/* including blinky files */
#include <pigpio.h>
#include <string.h>

/************* MACROS DEFINITIONS ************/
#define LED_RED			18
#define LED_GREEN		24
#define OUT3			16
#define INPUT_RED		2
#define INPUT_GREEN		3
#define INPUT3			5



int main(void) 
{ 	
	/* Server definitions */
	uint32_t sockfd = 0;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr; 
	uint32_t len = sizeof(struct sockaddr_in); 
	uint8_t Status = NACK;
	uint8_t* Frame = NULL;
	
	uint8_t StatusBuffer[STATUS_SIZE];
		
	FrameHeader_t TxFrameHeader = 
	{
		.Signature = SIGNATURE,
		.NumOfCommands = NUM_OF_CMD,
		.TotalDataSize = TOTAL_SIZE
	};
	
	FrameHeader_t RxFrameHeader;
	
	uint8_t FrameDataBuffer[512] = {0};
	//FrameData_t *RxFrameData = (FrameData_t *)FrameDataBuffer;
	
	/**************************** Initializations **********************************/
	/* Initializing the UDP connection*/
	UDP_ServerInit(&sockfd, &servaddr, &cliaddr);
	
	/* Initializing the GPIO for RPI */
	if (gpioInitialise() < 0)
	{
	   fprintf(stderr, "pigpio initialisation failed\n");
	}
	
	gpioSetMode(LED_RED, PI_OUTPUT);
	gpioSetMode(LED_GREEN, PI_OUTPUT);
	gpioSetMode(OUT3, PI_OUTPUT);
	
	gpioSetMode(INPUT_GREEN, PI_INPUT);
	gpioSetMode(INPUT_RED, PI_INPUT);
	gpioSetMode(INPUT3, PI_INPUT);

	
	/**************************** Receiving frames **********************************/
  
	/* HEADER */
	UDP_ServerReceive(&sockfd, (uint8_t*)&RxFrameHeader, &cliaddr, (uint32_t *)&len, sizeof(FrameHeader_t));
	
	if(RxFrameHeader.Signature == 0x07775000)
	{
		Status = ACK;
		printf("status sent: %d\n", Status);
		UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
		printf("Condition True\n");
		
		printf("Total Data size: %d\n", RxFrameHeader.TotalDataSize);		
		
		//~ double start = time_time();
		
		//~ while ((time_time() - start) < 5.0)
		//~ {
			//~ gpioWrite(LED_PIN, 1); /* on */ 
			//~ time_sleep(0.5);
			//~ gpioWrite(LED_PIN, 0); /* off */ 
			//~ time_sleep(0.5);
		//~ }
		
		UDP_ServerReceive(&sockfd, (uint8_t *)FrameDataBuffer, &cliaddr, (uint32_t *)&len, RxFrameHeader.TotalDataSize);
		
		for(uint16_t index =0; index<RxFrameHeader.TotalDataSize; index++)
		{
			printf("Byte[%d]: %d\n", index, FrameDataBuffer[index]);	
		}
		
		/********* UPDATING THE DIO PINS ********/
		
		/****ASSIGNING DIO FRAME****/
		
		//printf("DIO PERIPHERAL_ID: %d\n", RxFrameData->PeripheralID);
		//printf("DIO PERIPHERAL_DATA SIZE: %d\n", RxFrameData->DataSize);
		gpioWrite(LED_RED, FrameDataBuffer[4]);
		gpioWrite(LED_GREEN, FrameDataBuffer[5]);
		gpioWrite(OUT3, FrameDataBuffer[6]);

		
	printf("\n\n");
	}
	else
	{
		Status = NACK;
		UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
		printf("Condition False\n");
		gpioWrite(LED_RED, 0); /* off */	
	}
	
	
	/********** SENDING DATA TO PC ************/
	printf("/*********** SENDING DATA FROM RASPBERRY PI TO PC ***************/\n\n");
	
	FrameHeader_t temp_FrameHeader = 
	{
		.Signature 		= 	SIGNATURE,	
		.NumOfCommands	= 	1,
		.TotalDataSize	= 	11
	};
	
	UDP_ServerSend(&sockfd, (uint8_t *)&temp_FrameHeader, &cliaddr, len, sizeof(FrameHeader_t));
	//UDP_ServerSend(&sockfd, (uint8_t *)&TxFrameHeader, &cliaddr, len, sizeof(FrameHeader_t));
	
	for(uint32_t i; i < sizeof(FrameHeader_t); i++)
	{
		printf("Byte[%d]: %d\n", i, ((uint8_t *)&TxFrameHeader)[i]);
	}
	
	UDP_ServerReceive(&sockfd, StatusBuffer, &cliaddr, (uint32_t *)&len, STATUS_SIZE);
	
	if(StatusBuffer[0] == ACK)
	{
		printf("Easy-PC.. Haha\n");
		
		
		//Frame = FRAME_Generate();
		
		//~ uint32_t DIO_id = 0x57;
		//~ uint32_t DIO_datasize = 2;
		//~ uint8_t DIO_Frame[2] = {0};

		uint8_t zart[11] = {0};
		zart[0] = 0x15;
		zart[1] = 0x45;
		zart[2] = 0;
		zart[3] = 0;
		zart[4] = 2;
		zart[5] = 0;
		zart[6] = 0;
		zart[7] = 0;
		zart[8] = gpioRead(INPUT_RED);
		zart[9] = gpioRead(INPUT_GREEN);
		zart[10] = gpioRead(INPUT3);
		
		
		UDP_ServerSend(&sockfd, (uint8_t*)zart, &cliaddr, len, temp_FrameHeader.TotalDataSize);
		//UDP_ServerSend(&sockfd, (uint8_t*)Frame, &cliaddr, len, TxFrameHeader.TotalDataSize);
		
		printf("DATA FRAME SENT: \n");
		for(uint32_t i; i < temp_FrameHeader.TotalDataSize; i++)
		{
			printf("Byte[%d]: %d\n", i, zart[i]);
		}	
		
		//~ for(uint32_t i; i < TxFrameHeader.TotalDataSize; i++)
		//~ {
			//~ printf("Byte[%d]: %d\n", i, Frame[i]);
		//~ }	
	}
	else
	{
		printf("PC - yad\n");
	}
	
	/*************************** Disconnecting the UDP connection *********************/
	UDP_ServerDisconnect(&sockfd);
	
	gpioTerminate();
	return 0;
}

