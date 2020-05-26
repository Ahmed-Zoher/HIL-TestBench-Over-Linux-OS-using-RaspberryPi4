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
	
	Test_frame frame = 
	{
		.ID 	= 0,
		.CMD	= 0,
		.Ay7aga	= 0
	};
	
	
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
  
	UDP_ReceiveFrame(&sockfd, &frame, &cliaddr, (uint32_t *)&len);
	UDP_SendACK(&sockfd, &cliaddr, len);
	

	if(frame.ID == 5)
	{
		printf("Condition True\n");
		double start = time_time();
		while ((time_time() - start) < 30.0)
		{
			gpioWrite(LED_PIN, 1); /* on */ 
			time_sleep(0.5);
			gpioWrite(LED_PIN, 0); /* off */ 
			time_sleep(0.5);
		}
	}
	else
	{
		printf("Condition False\n");
		gpioWrite(LED_PIN, 0); /* off */	
	}
	
	
	/*************************** Disconnecting the UDP connection *******************/
	UDP_Disconnect(&sockfd);
	return 0;
}
