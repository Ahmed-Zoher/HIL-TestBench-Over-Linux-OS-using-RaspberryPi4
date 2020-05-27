/**
 *  @file Application.c
 *  @author Hazem Mekawy
 *  @brief Linux Server Application
 */

#include "server.h"

/* including blinky files */
#include <pigpio.h>

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
	
	FrameHeader_t FrameHeader;
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
	UDP_ServerReceive(&sockfd, (uint8_t*)&FrameHeader, &cliaddr, (uint32_t *)&len, sizeof(FrameHeader_t));
	
	if(FrameHeader.Signature == 0x07775000)
	{
		UDP_ServerSend(&sockfd, ACK_arr, &cliaddr, len, ACK_SIZE);
		printf("Condition True\n");
		
		printf("Total Data size: %d\n", FrameHeader.TotalDataSize);		
		
		
		double start = time_time();
		while ((time_time() - start) < 5.0)
		{
			gpioWrite(LED_PIN, 1); /* on */ 
			time_sleep(0.5);
			gpioWrite(LED_PIN, 0); /* off */ 
			time_sleep(0.5);
		}
		UDP_ServerReceive(&sockfd, (uint8_t *)FrameDataBuffer, &cliaddr, (uint32_t *)&len, FrameHeader.TotalDataSize);
		
		
		for(uint16_t index =0; index<FrameHeader.TotalDataSize; index++)
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
	
	/*************************** Disconnecting the UDP connection *******************/
	UDP_ServerDisconnect(&sockfd);
	return 0;
}
