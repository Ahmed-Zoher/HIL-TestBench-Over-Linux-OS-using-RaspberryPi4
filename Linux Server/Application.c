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
#define OUT1			25
#define OUT2			24
#define OUT3			16

#define INPUT1			2
#define INPUT2			3
#define INPUT3			5

#define PWM_GPIO		18
#define PWM2_GPIO		13

int main(void) 
{ 	
	/* Server definitions */
	uint32_t sockfd = 0;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr; 
	uint32_t len = sizeof(struct sockaddr_in); 
	uint8_t Status = NACK;
	//uint8_t* Frame = NULL;
	uint32_t ProgramCounter = 0;
	
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
	   return 1;
	}
	
	gpioSetMode(OUT1, PI_OUTPUT);
	gpioSetMode(OUT2, PI_OUTPUT);
	gpioSetMode(OUT3, PI_OUTPUT);
	
	gpioSetMode(PWM_GPIO, PI_OUTPUT);
	gpioSetMode(PWM2_GPIO, PI_OUTPUT);
	
	gpioSetMode(INPUT1, PI_INPUT);
	gpioSetMode(INPUT2, PI_INPUT);
	gpioSetMode(INPUT3, PI_INPUT);
	
	while(1)
	{
		ProgramCounter++;
		printf("\n***********Program Count: %d***********\n", ProgramCounter);
		
		/**************************** Receiving frames **********************************/
		/* Receiving Header */
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
			gpioWrite(OUT1, FrameDataBuffer[8]);
			gpioWrite(OUT2, FrameDataBuffer[9]);
			gpioWrite(OUT3, FrameDataBuffer[10]);
			
			uint32_t Frequency1 = *((uint32_t *)(FrameDataBuffer+19));
			uint32_t DutyCycle1 = *((uint32_t *)(FrameDataBuffer+23));
			
			uint32_t Frequency2 = *((uint32_t *)(FrameDataBuffer+27));
			uint32_t DutyCycle2 = *((uint32_t *)(FrameDataBuffer+31));
			
			printf("Frequency1: %d\tDutyCycle1: %d\n", Frequency1, DutyCycle1);
			printf("Frequency2: %d\tDutyCycle2: %d\n", Frequency2, DutyCycle2);
			
			printf("Running Motors\n");
			
		
			
			gpioHardwarePWM(PWM_GPIO, Frequency1, DutyCycle1);
			gpioHardwarePWM(PWM2_GPIO, Frequency2, DutyCycle2);
			
		printf("\n\n");
		}
		else
		{
			Status = NACK;
			UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
			printf("Condition False\n");
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
			//~ uint32_t DIO_datasize = 3;
			//~ uint8_t DIO_Frame[2] = {0};

			uint8_t zart[11] = {0};
			zart[0] = 0x15;
			zart[1] = 0x45;
			zart[2] = 0;
			zart[3] = 0;
			zart[4] = 3;
			zart[5] = 0;
			zart[6] = 0;
			zart[7] = 0;
			zart[8] = gpioRead(INPUT1);
			zart[9] = gpioRead(INPUT2);
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
	}
	
	
	/*************************** Disconnecting the UDP connection *********************/
	UDP_ServerDisconnect(&sockfd);
	
	gpioTerminate();
	return 0;
}

