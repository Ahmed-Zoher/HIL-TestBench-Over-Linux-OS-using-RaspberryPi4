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
#define LED_PIN		18

int main(void) 
{ 	
	/* Server definitions */
	uint32_t sockfd = 0;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr; 
	uint32_t len = sizeof(struct sockaddr_in); 
	uint8_t ACK_arr[] = "ACK";
	uint8_t NACK_arr[] = "NACK";
	uint8_t* Frame = NULL;
	
	//~ uint8_t buf[100];
	uint8_t recvBuf[ACK_SIZE];
	
	FrameHeader_t RxFrameHeader;
		
	FrameHeader_t TxFrameHeader = 
	{
		.Signature = SIGNATURE,
		.NumOfCommands = NUM_OF_CMD,
		.TotalDataSize = TOTAL_SIZE
	};
	
	
	uint8_t FrameDataBuffer[100] = {0};
	
	/**************************** Initializations **********************************/
	/* Initializing the UDP connection*/
	UDP_ServerInit(&sockfd, &servaddr, &cliaddr);
	
	/* Initializing the GPIO for RPI */
	if (gpioInitialise() < 0)
	{
	   fprintf(stderr, "pigpio initialisation failed\n");
	}
	
	/* Configuring the GPIo pins */
	gpioSetMode(LED_PIN, PI_OUTPUT);
	
	
	/**************************** Receiving frames **********************************/
  
	/* HEADER */
	UDP_ServerReceive(&sockfd, (uint8_t*)&RxFrameHeader, &cliaddr, (uint32_t *)&len, sizeof(FrameHeader_t));
	
	if(RxFrameHeader.Signature == 0x07775000)
	{
		UDP_ServerSend(&sockfd, ACK_arr, &cliaddr, len, ACK_SIZE);
		printf("Condition True\n");
		
		printf("Total Data size: %d\n", RxFrameHeader.TotalDataSize);		
		
		
		double start = time_time();
		
		while ((time_time() - start) < 5.0)
		{
			gpioWrite(LED_PIN, 1); /* on */ 
			time_sleep(0.5);
			gpioWrite(LED_PIN, 0); /* off */ 
			time_sleep(0.5);
		}
		
		
		
		UDP_ServerReceive(&sockfd, (uint8_t *)FrameDataBuffer, &cliaddr, (uint32_t *)&len, RxFrameHeader.TotalDataSize);
		for(uint16_t index =0; index<RxFrameHeader.TotalDataSize; index++)
		{
			printf("Byte[%d]: %d\n", index, FrameDataBuffer[index]);	
		}
		
	printf("\n\n");
	}
	else
	{
		UDP_ServerSend(&sockfd, NACK_arr, &cliaddr, len, NACK_SIZE);
		printf("Condition False\n");
		gpioWrite(LED_PIN, 0); /* off */	
	}

	/********** SENDING DATA TO PC ************/
	UDP_ServerSend(&sockfd, (uint8_t *)&TxFrameHeader, &cliaddr, len, sizeof(FrameHeader_t));
	UDP_ServerReceive(&sockfd, recvBuf, &cliaddr, (uint32_t *)&len, ACK_SIZE);
	
	printf("HERE\n");
	
	if(strcmp((const char *)recvBuf, "ACK") == 0)
	{
		printf("Easy-PC.. Haha\n");
		
		Frame = FRAME_Generate();
		UDP_ServerSend(&sockfd, (uint8_t*)Frame, &cliaddr, len, TxFrameHeader.TotalDataSize);	
	}
	else
	{
		printf("PC - yad\n");
	}
	

	/*************************** Disconnecting the UDP connection *********************/
	UDP_ServerDisconnect(&sockfd);

	return 0;
}

