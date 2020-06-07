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
#define ERROR_PIGPIO_INIT			1
#define ERROR_PIGPIO_SERIAL_INIT	2

#define OUT1			25
#define OUT2			24
#define OUT3			16

#define INPUT1			2
#define INPUT2			3
#define INPUT3			5

#define PWM_GPIO		18
#define PWM2_GPIO		13

#define PIN_TX	14
#define PIN_RX	15

#define DEFAULT_UART_HANDLER	"/dev/ttyAMA0"
#define RPI_BAUD_RATE			9600

uint8_t UART_buffer[100] = {0};
uint8_t SPI_CH1_buffer[100] = {0};
uint8_t SPI_CH2_buffer[100] = {0};

typedef struct
{
	uint32_t SerialStatus;
	uint32_t SerialBaudrate;
	uint32_t SerialDataSize;
	int	SerialHandle;	
	uint8_t *Buffer;
}SerialInfo_t;

extern uint8_t ClientAvailable;

SerialInfo_t UART_Info;
SerialInfo_t SPI_CH1_Info;
SerialInfo_t SPI_CH2_Info;

int main(void) 
{ 	
	UART_Info.Buffer = (uint8_t *)UART_buffer;
	SPI_CH1_Info.Buffer = (uint8_t *)SPI_CH1_buffer;
	SPI_CH2_Info.Buffer = (uint8_t *)SPI_CH2_buffer;
	
	/* Server definitions */
	uint32_t sockfd = 0;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr; 
	uint32_t len = sizeof(struct sockaddr_in); 
	uint8_t Status = NACK;
	//uint8_t* Frame = NULL;
	uint32_t ProgramCounter = 0;
	uint32_t Rx_Counter = 0;
	
	uint8_t StatusBuffer[STATUS_SIZE];
	uint16_t index = 0;
	
	FrameHeader_t TxFrameHeader = 
	{
		.Signature = SIGNATURE,
		.NumOfCommands = NUM_OF_CMD,
		.TotalDataSize = TOTAL_SIZE
	};
	
	FrameHeader_t RxFrameHeader;
	
	uint8_t FrameDataBuffer[512] = {0};
	//FrameData_t *RxFrameData = (FrameData_t *)FrameDataBuffer;
	
	
	/* Initializing the UDP connection*/
	UDP_ServerInit(&sockfd, &servaddr, &cliaddr);
	
	
	while(1)
	{
		if(UDP_ValidateKey(&sockfd, &servaddr, &cliaddr) == 1)
		{
			while(ClientAvailable)
			{
				/**************************** Initializations **********************************/
				/* Initializing the GPIO for RPI */
				if (gpioInitialise() < 0)
				{
				   fprintf(stderr, "pigpio initialisation failed\n");
				   return ERROR_PIGPIO_INIT;
				}
				
				gpioSetMode(OUT1, PI_OUTPUT);
				gpioSetMode(OUT2, PI_OUTPUT);
				gpioSetMode(OUT3, PI_OUTPUT);
				
				gpioSetMode(PWM_GPIO, PI_OUTPUT);
				gpioSetMode(PWM2_GPIO, PI_OUTPUT);
				
				gpioSetMode(INPUT1, PI_INPUT);
				gpioSetMode(INPUT2, PI_INPUT);
				gpioSetMode(INPUT3, PI_INPUT);
				
				ProgramCounter++;
				printf("\n***********Program Count: %d***********\n", ProgramCounter);
				
				/**************************** Receiving frames **********************************/
				/* Receiving Header */
				if(ClientAvailable)
					UDP_ServerReceive(&sockfd, (uint8_t*)&RxFrameHeader, &cliaddr, (uint32_t *)&len, sizeof(FrameHeader_t));
				else
					break;
				
				
				printf("RECEIVING RX HEADER\n");
				for(index =0; index<sizeof(FrameHeader_t); index++)
				{
					printf("Rx_FrameHeader_Byte[%d]: %d\n", index, ((uint8_t*)&RxFrameHeader)[index]);	
				}
				
				
				if(RxFrameHeader.Signature == 0x07775000)
				{
					Status = ACK;
					UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
					printf("Sent Rx_FrameHeader Status: %d\n", Status);
					printf("Signature is Valid\n");
					
					printf("Total Data size(Rx_FrameHeader): %d\n", RxFrameHeader.TotalDataSize);		
					
					if(ClientAvailable)
						UDP_ServerReceive(&sockfd, (uint8_t *)FrameDataBuffer, &cliaddr, (uint32_t *)&len, RxFrameHeader.TotalDataSize);
					else
						break;
					
					
					for(index =0; index<RxFrameHeader.TotalDataSize; index++)
					{
						printf("Rx_DataFrame_Byte[%d]: %d\n", index, FrameDataBuffer[index]);	
					}
					
					/********* UPDATING THE DIO PINS ********/
					/****ASSIGNING DIO FRAME****/
					printf("SETTING THE DIO PUTPUT\n");
					gpioWrite(OUT1, FrameDataBuffer[8]);
					gpioWrite(OUT2, FrameDataBuffer[9]);
					gpioWrite(OUT3, FrameDataBuffer[10]);
					
					uint32_t Frequency1 = *((uint32_t *)(FrameDataBuffer+19));
					uint32_t DutyCycle1 = *((uint32_t *)(FrameDataBuffer+23));
					
					uint32_t Frequency2 = *((uint32_t *)(FrameDataBuffer+27));
					uint32_t DutyCycle2 = *((uint32_t *)(FrameDataBuffer+31));
					
					printf("PWM Info read\n");
					printf("Frequency1: %d\tDutyCycle1: %d\n", Frequency1, DutyCycle1);
					printf("Frequency2: %d\tDutyCycle2: %d\n", Frequency2, DutyCycle2);		
					gpioHardwarePWM(PWM_GPIO, Frequency1, DutyCycle1);
					gpioHardwarePWM(PWM2_GPIO, Frequency2, DutyCycle2);
					
					
					/////////////////////// UART CHANNEL /////////////////////			
					////////UART Info
					printf("UART Info read\n");
					UART_Info.SerialStatus = *((uint32_t *)(FrameDataBuffer+43));
					UART_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+47));
					UART_Info.SerialDataSize = *((uint32_t *)(FrameDataBuffer+51));
					
					
					/////////////////////// SPI CHANNELS /////////////////////
					////////SPI_CH1 Info
					printf("SPI_CH1 Info read\n");
					SPI_CH1_Info.SerialStatus = *((uint32_t *)(FrameDataBuffer+59));
					SPI_CH1_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+63));
					SPI_CH1_Info.SerialDataSize = *((uint32_t *)(FrameDataBuffer+67));
					
					////////SPI_CH2 Info
					printf("SPI_CH2 Info read\n");
					SPI_CH2_Info.SerialStatus = *((uint32_t *)(FrameDataBuffer+75));
					SPI_CH2_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+79));
					SPI_CH2_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+83));
					
				printf("\n\n");
				}
				else
				{
					Status = NACK;
					UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
					printf("Sent Rx_FrameHeader Status: %d\n", Status);
					
					printf("Signature Invalid\n");
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
					printf("Tx_FrameHeader_Byte[%d]: %d\n", i, ((uint8_t *)&TxFrameHeader)[i]);
				}
				
				if(ClientAvailable)
					UDP_ServerReceive(&sockfd, StatusBuffer, &cliaddr, (uint32_t *)&len, STATUS_SIZE);
				else
					break;
				
				if(StatusBuffer[0] == ACK)
				{
					printf("RECEIVED AN ACK FROM PC TO SEND RPI TX DATA\n");
							
					//Frame = FRAME_Generate();

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
					for(uint32_t i = 0; i < temp_FrameHeader.TotalDataSize; i++)
					{
						printf("Tx_FrameData_Byte[%d]: %d\n", i, zart[i]);
					}	
					
					//~ for(uint32_t i; i < TxFrameHeader.TotalDataSize; i++)
					//~ {
						//~ printf("Byte[%d]: %d\n", i, Frame[i]);
					//~ }	
				}
				else
				{
					printf("PC - yad\n");
					printf("GOT A NACK FROM PC FROM THE TX FRAME HEADER\n");
				}
				
				////////////////////////////////////// COMMUNICATION PROTOCOLS ///////////////////////////////
				/////////////////////// RECEIVING DATA ON UART /////////////////////	
				if(UART_Info.SerialStatus == 1)
				{
					UART_Info.SerialHandle = serOpen(DEFAULT_UART_HANDLER, UART_Info.SerialBaudrate, 0);
					
					if(UART_Info.SerialHandle < 0)
					{
						fprintf(stderr, "Serial port initialisation for TX failed\n");
						return ERROR_PIGPIO_SERIAL_INIT;	
					}
					if(UART_Info.SerialDataSize > 0)
					{
						if(ClientAvailable)
							UDP_ServerReceive(&sockfd, UART_Info.Buffer, &cliaddr, (uint32_t *)&len, UART_Info.SerialDataSize);
						else
							break;
					}
						
					for(uint32_t i = 0; i < UART_Info.SerialDataSize ; i++)
					{
						printf("SERIAL_RX[%d]: %02X\n", i, UART_Info.Buffer[i]);
					}
					
					//Sending data over UART
					serWrite(UART_Info.SerialHandle, (char *)UART_Info.Buffer, UART_Info.SerialDataSize);
					gpioDelay(100000); //100msec
					
					//Closing the TX UART Handle
					//serClose(UART_Info.SerialHandle);
					
					//Opening RX UART Handle
					//UART_Info.SerialHandle = serOpen(DEFAULT_UART_HANDLER, UART_Info.SerialBaudrate, 0);

					//~ if(UART_Info.SerialHandle < 0)
					//~ {
						//~ fprintf(stderr, "Serial port initialisation for RX failed\n");
						//~ return ERROR_PIGPIO_SERIAL_INIT;	
					//~ }
					
					Rx_Counter = 0;
					while(serDataAvailable(UART_Info.SerialHandle) > 0)
					{
						UART_Info.Buffer[Rx_Counter] = serReadByte(UART_Info.SerialHandle);
						Rx_Counter++;
						printf("Counter: %d\n", Rx_Counter);
					}
					
					for(int i = 0; i < Rx_Counter; i++)
					{
						printf("BUFFER[%d]: %d\n", i, UART_Info.Buffer[i]);
					}
					
					UDP_ServerSend(&sockfd, (uint8_t*)&Rx_Counter, &cliaddr, len, sizeof(uint32_t));
					UDP_ServerSend(&sockfd, (uint8_t*)UART_Info.Buffer, &cliaddr, len, UART_Info.SerialDataSize);
				}
				if(UART_Info.SerialHandle > 0)
					serClose(UART_Info.SerialHandle);
				
				gpioTerminate();
			}
		}
		
	}
	
	/*************************** Disconnecting the UDP connection *********************/
	UDP_ServerDisconnect(&sockfd);
	

	return 0;
}

