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
#define ERROR_PIGPIO_INIT				1
#define ERROR_PIGPIO_UART_INIT			2
#define ERROR_PIGPIO_SPI_CH1_INIT		3
#define ERROR_PIGPIO_SPI_CH2_INIT		4


#define OUT1				24
#define OUT2				25
#define OUT3				26
#define OUT4				27

#define INPUT1				5
#define INPUT2				6
#define INPUT3				22
#define INPUT4				23
	
#define PWM0_OUT			12
#define PWM1_OUT			13

#define PWM0_INPUT			0
#define PWM1_INPUT			1
	
#define UART_TX				14	
#define UART_RX				15	
	
#define SPI0_MOSI			10
#define SPI0_MISO			9
#define SPI0_SCLK			11
#define SPI0_CE0			8
#define SPI0_CE1			7
	
	
#define SPI1_MOSI			20
#define SPI1_MISO			19
#define SPI1_SCLK			21
#define SPI1_CE0			18
#define SPI1_CE1			17
#define SPI1_CE2			16
	
#define I2C_SDA				2
#define I2C_CLK				3

#define DEFAULT_UART_HANDLER	"/dev/ttyAMA0"
#define RPI_BAUD_RATE			9600

#define SPI1_CHANNEL			0
#define SPI1_FLAGS				0x00

#define SPI2_CHANNEL			1
#define SPI2_FLAGS				0x1E0

#define RX_BUFF_LEN		512

uint8_t UART_Tx_Buffer[RX_BUFF_LEN] = {0};
uint8_t UART_Rx_Buffer[RX_BUFF_LEN] = {0};
uint8_t SPI_CH1_Tx_Buffer[RX_BUFF_LEN] = {0};
uint8_t SPI_CH1_Rx_Buffer[RX_BUFF_LEN] = {0};
uint8_t SPI_CH2_Tx_Buffer[RX_BUFF_LEN] = {0};
uint8_t SPI_CH2_Rx_Buffer[RX_BUFF_LEN] = {0};

typedef struct
{
	uint32_t SerialStatus;
	uint32_t SerialBaudrate;
	uint32_t SerialDataSize;
	int	SerialHandle;	
	uint8_t *Tx_Buffer;
	uint8_t *Rx_Buffer;
}SerialInfo_t;

extern uint8_t ClientAvailable;




SerialInfo_t UART_Info = {
	.SerialStatus 		= 0,
	.SerialBaudrate 	= 0,
	.SerialDataSize		= 0,
	.SerialHandle		= -1,	
	.Tx_Buffer			= UART_Tx_Buffer,
	.Rx_Buffer			= UART_Rx_Buffer
	
};

SerialInfo_t SPI_CH1_Info = {
	.SerialStatus 		= 0,
	.SerialBaudrate 	= 0,
	.SerialDataSize		= 0,
	.SerialHandle		= -1,	
	.Tx_Buffer			= SPI_CH1_Tx_Buffer,
	.Rx_Buffer			= SPI_CH1_Rx_Buffer
};

SerialInfo_t SPI_CH2_Info= {
	.SerialStatus 		= 0,
	.SerialBaudrate 	= 0,
	.SerialDataSize		= 0,
	.SerialHandle		= -1,	
	.Tx_Buffer			= SPI_CH2_Tx_Buffer,
	.Rx_Buffer			= SPI_CH2_Rx_Buffer
};

////////////PWM statics/////////
static uint32_t rise_tick1 = 0;    // Pulse rise time tick value
static uint32_t On_Time1 = 0;  // Last measured pulse width (us)

static uint32_t falling_tick1 = 0;
static uint32_t Off_Time1 = 0;

uint32_t DutyCycleReading1 = 0;
uint32_t FrequencyReading1 = 0;

static uint32_t rise_tick2 = 0;    // Pulse rise time tick value
static uint32_t On_Time2 = 0;  // Last measured pulse width (us)

static uint32_t falling_tick2 = 0;
static uint32_t Off_Time2 = 0;

uint32_t DutyCycleReading2 = 0;
uint32_t FrequencyReading2 = 0;


///////////TO BE REMMOVED RX //////
#define DIO_ID	0x03
#define PWM_ID  0x04
#define DIO_SIZE	16
#define PWM_SIZE	16

uint32_t DIO_Id = DIO_ID;
uint32_t DIOSize= DIO_SIZE;
uint32_t PWM_Id = PWM_ID;
uint32_t PWMSize= PWM_SIZE;
uint32_t zart[12] = {0};
uint32_t ProgramCounter = 0;
// Callback function for measuring PWM input
void pwm1_cbfunc(int user_gpio, int level, uint32_t tick)
 {
    if (level == 1) 
	{  
		// rising edge
        rise_tick1 = tick;
		Off_Time1 = tick - falling_tick1;
    }
    else if (level == 0) 
	{  
		// falling edge
        On_Time1 = tick - rise_tick1;  // TODO: Handle 72 min wrap-around
		falling_tick1 = tick;
    }
}

void pwm2_cbfunc(int user_gpio, int level, uint32_t tick)
 {
    if (level == 1) 
	{  
		// rising edge
        rise_tick2 = tick;
		Off_Time2 = tick - falling_tick2;
    }
    else if (level == 0) 
	{  
		// falling edge
        On_Time2 = tick - rise_tick2;  // TODO: Handle 72 min wrap-around
		falling_tick2 = tick;
    }
}

int gpio_Init(void)
{
	if (gpioInitialise() < 0)
	{
		fprintf(stderr, "pigpio initialisation failed\n");
		return ERROR_PIGPIO_INIT;
	}
	
	/**************************** Initializations **********************************/
	/* Setting Pin Modes */
	gpioSetMode(OUT1, PI_OUTPUT);
	gpioSetMode(OUT2, PI_OUTPUT);
	gpioSetMode(OUT3, PI_OUTPUT);
	gpioSetMode(OUT4, PI_OUTPUT);
	
	gpioSetMode(PWM0_OUT, PI_OUTPUT);
	gpioSetMode(PWM1_OUT, PI_OUTPUT);
				
	gpioSetMode(INPUT1, PI_INPUT);
	gpioSetMode(INPUT2, PI_INPUT);
	gpioSetMode(INPUT3, PI_INPUT);
	gpioSetMode(INPUT4, PI_INPUT);
				
				
	gpioSetMode(PWM0_INPUT, PI_INPUT);
	gpioSetMode(PWM1_INPUT, PI_INPUT);
	
	// Set up callback for PWM input 
	gpioSetAlertFunc(PWM0_INPUT, pwm1_cbfunc);
	gpioSetAlertFunc(PWM1_INPUT, pwm2_cbfunc);
	return 0;
}

void gpio_DeInit(void)
{
	ProgramCounter = 0;
	//gpioTerminate();
}


int main(void) 
{ 		
	/* Server definitions */
	uint32_t sockfd = 0;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr; 
	uint32_t len = sizeof(struct sockaddr_in); 
	uint8_t Status = NACK;
	//uint8_t* Frame = NULL;
	
	uint32_t Rx_Counter = 0;
	uint32_t index = 0;
	uint8_t gpioInit_state = 0;
	
	uint8_t StatusBuffer[STATUS_SIZE];
	//~ uint16_t index = 0;
	
	//~ FrameHeader_t TxFrameHeader = 
	//~ {
		//~ .Signature = SIGNATURE,
		//~ .NumOfCommands = NUM_OF_CMD,
		//~ .TotalDataSize = TOTAL_SIZE
	//~ };
	
	FrameHeader_t RxFrameHeader;
	
	uint8_t FrameDataBuffer[512] = {0};
	//FrameData_t *RxFrameData = (FrameData_t *)FrameDataBuffer;
	
	//Init the gpio
	gpioInit_state = gpio_Init();
	
	/* Initializing the UDP connection*/
	UDP_ServerInit(&sockfd, &servaddr, &cliaddr);
	
	
	while(gpioInit_state == 0)
	{
		if(UDP_ValidateKey(&sockfd, &servaddr, &cliaddr) == 1)
		{
			while(ClientAvailable)
			{
				ProgramCounter++;
				printf("\n***********Program Count: %d***********\n", ProgramCounter);
				
				/**************************** Receiving frames **********************************/
				/* Receiving Header */
				if(ClientAvailable)
				{
					UDP_ServerReceive(&sockfd, (uint8_t*)&RxFrameHeader, &cliaddr, (uint32_t *)&len, sizeof(FrameHeader_t));
				}
				else
				{
					gpio_DeInit();
					break;
				}
				
				//Printing header frame received from the PC
				printf("RECEIVING RX HEADER\n");
				for(index =0; index<sizeof(FrameHeader_t); index++)
				{
					printf("Rx_FrameHeader_Byte[%d]: %d\n", index, ((uint8_t*)&RxFrameHeader)[index]);	
				}
				
				
				if(RxFrameHeader.Signature == 0x07775000)
				{
					Status = ACK;
					UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
					//~ printf("Sent Rx_FrameHeader Status: %d\n", Status);
					printf("Signature is Valid\n");
					
					printf("Total Data size(Rx_FrameHeader): %d\n", RxFrameHeader.TotalDataSize);		
					
					if(ClientAvailable)
					{
						UDP_ServerReceive(&sockfd, (uint8_t *)FrameDataBuffer, &cliaddr, (uint32_t *)&len, RxFrameHeader.TotalDataSize);
					}
					else
					{
						gpio_DeInit();
						break;
					}
					
					//Printing the FRAME received from PC
					for(index =0; index<RxFrameHeader.TotalDataSize; index++)
					{
						printf("Rx_DataFrame_Byte[%d]: %d\n", index, FrameDataBuffer[index]);	
					}
					
					/********* UPDATING THE DIO PINS ********/
					/****ASSIGNING DIO FRAME****/
					//~ printf("SETTING THE DIO PUTPUT\n");
					gpioWrite(OUT1, FrameDataBuffer[8]);
					gpioWrite(OUT2, FrameDataBuffer[9]);
					gpioWrite(OUT3, FrameDataBuffer[10]);
					gpioWrite(OUT4, FrameDataBuffer[11]);
					
					uint32_t Frequency1 = *((uint32_t *)(FrameDataBuffer+20));
					uint32_t DutyCycle1 = *((uint32_t *)(FrameDataBuffer+24));
					
					uint32_t Frequency2 = *((uint32_t *)(FrameDataBuffer+28));
					uint32_t DutyCycle2 = *((uint32_t *)(FrameDataBuffer+32));
					
					printf("PWM Info read\n");
					printf("Frequency1: %d\tDutyCycle1: %d\n", Frequency1, DutyCycle1);
					printf("Frequency2: %d\tDutyCycle2: %d\n", Frequency2, DutyCycle2);		
					gpioHardwarePWM(PWM0_OUT, Frequency1, DutyCycle1);
					gpioHardwarePWM(PWM1_OUT, Frequency2, DutyCycle2);
					
					
					/////////////////////// UART CHANNEL /////////////////////			
					////////UART Info
					//~ printf("UART Info read\n");
					UART_Info.SerialStatus = *((uint32_t *)(FrameDataBuffer+44));
					UART_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+48));
					UART_Info.SerialDataSize = *((uint32_t *)(FrameDataBuffer+52));
					printf("*********** UART Info ***********\n");
					printf("UART_SerialStatus: %d\n", UART_Info.SerialStatus);
					printf("UART_SerialBaudrate: %d\n", UART_Info.SerialBaudrate);	
					printf("UART_SerialDataSize: %d\n", UART_Info.SerialDataSize);
					
					/////////////////////// SPI CHANNELS /////////////////////
					////////SPI_CH1 Info
					//~ printf("SPI_CH1 Info read\n");
					SPI_CH1_Info.SerialStatus = *((uint32_t *)(FrameDataBuffer+64));
					SPI_CH1_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+68));
					SPI_CH1_Info.SerialDataSize = *((uint32_t *)(FrameDataBuffer+72));
					printf("*********** SPI_CH1 Info ***********\n");
					printf("SPI_CH1_SerialStatus: %d\n", SPI_CH1_Info.SerialStatus);
					printf("SPI_CH1_SerialBaudrate: %d\n", SPI_CH1_Info.SerialBaudrate);	
					printf("SPI_CH1_SerialDataSize: %d\n", SPI_CH1_Info.SerialDataSize);	
					
									
					////////SPI_CH2 Info
					//~ printf("SPI_CH2 Info read\n");
					SPI_CH2_Info.SerialStatus = *((uint32_t *)(FrameDataBuffer+84));
					SPI_CH2_Info.SerialBaudrate = *((uint32_t *)(FrameDataBuffer+88));
					SPI_CH2_Info.SerialDataSize = *((uint32_t *)(FrameDataBuffer+92));
					printf("*********** SPI_CH2 Info ***********\n");
					printf("SPI_CH2_SerialStatus: %d\n", SPI_CH2_Info.SerialStatus);
					printf("SPI_CH2_SerialBaudrate: %d\n", SPI_CH2_Info.SerialBaudrate);	
					printf("SPI_CH2_SerialDataSize: %d\n", SPI_CH2_Info.SerialDataSize);					
					printf("\n\n");
				
				}
				else
				{
					Status = NACK;
					UDP_ServerSend(&sockfd, &Status, &cliaddr, len, STATUS_SIZE);
					//~ printf("Sent Rx_FrameHeader Status: %d\n", Status);
					printf("Signature Invalid\n");
				}
				
				/********** SENDING DATA TO PC ************/
				printf("/*********** SENDING DATA FROM RASPBERRY PI TO PC ***************/\n\n");
				
				FrameHeader_t temp_FrameHeader = 
				{
					.Signature 		= 	SIGNATURE,	
					.NumOfCommands	= 	2,
					.TotalDataSize	= 	48
				};
				
				UDP_ServerSend(&sockfd, (uint8_t *)&temp_FrameHeader, &cliaddr, len, sizeof(FrameHeader_t));
				//UDP_ServerSend(&sockfd, (uint8_t *)&TxFrameHeader, &cliaddr, len, sizeof(FrameHeader_t));
				
				//~ for(uint32_t i; i < sizeof(FrameHeader_t); i++)
				//~ {
					//~ printf("Tx_FrameHeader_Byte[%d]: %d\n", i, ((uint8_t *)&TxFrameHeader)[i]);
				//~ }
				
				if(ClientAvailable)
				{
					UDP_ServerReceive(&sockfd, StatusBuffer, &cliaddr, (uint32_t *)&len, STATUS_SIZE);
				
				}
				else
				{
					gpio_DeInit();
					break;
				}
				
				if(StatusBuffer[0] == ACK)
				{
					//~ printf("RECEIVED AN ACK FROM PC TO SEND RPI TX DATA\n");
							
					//Frame = FRAME_Generate();
					
					//~ printf("ON_TIME1: %d\t OFF_TIME1: %d\n", On_Time1, Off_Time1);
					if((On_Time1 == 0) && (Off_Time1 == 0))
					{
						DutyCycleReading1 = 0;
						FrequencyReading1 = 0;
					}
					else
					{
						DutyCycleReading1 = ((On_Time1 * 100) / (On_Time1+Off_Time1));
						FrequencyReading1 = (1000000 / (On_Time1+Off_Time1)) ;
					}
					//~ printf("DutyCycleReading1: %d\tFrequencyReading1: %d\n", 
					//~ DutyCycleReading1, FrequencyReading1);
					
					//~ printf("ON_TIME2: %d\t OFF_TIME2: %d\n", On_Time2, Off_Time2);
					if((On_Time2 == 0) && (Off_Time2 == 0))
					{
						DutyCycleReading2 = 0;
						FrequencyReading2 = 0;
					}
					else
					{
						DutyCycleReading2 = ((On_Time2 * 100)/ (On_Time2+Off_Time2)) ;
						FrequencyReading2 = ((1000000 / (On_Time2+Off_Time2))) ;
					}
					//~ printf("DutyCycleReading2: %d\tFrequencyReading2: %d\n", 
					//~ DutyCycleReading2, FrequencyReading2);

								
					memset(zart, 0, 12*sizeof(uint32_t));
					zart[0] = DIO_Id;
					zart[1] = DIOSize;
					zart[2] = gpioRead(INPUT1);
					zart[3] = gpioRead(INPUT2);
					zart[4] = gpioRead(INPUT3);
					zart[5] = gpioRead(INPUT4);
					zart[6] = PWM_Id;
					zart[7] = PWMSize;
					zart[8] = DutyCycleReading1;
					zart[9] = FrequencyReading1;
					zart[10] = DutyCycleReading2;
					zart[11] = FrequencyReading2;


					UDP_ServerSend(&sockfd, (uint8_t*)zart, &cliaddr, len, temp_FrameHeader.TotalDataSize);
					//UDP_ServerSend(&sockfd, (uint8_t*)Frame, &cliaddr, len, TxFrameHeader.TotalDataSize);
					
					//~ printf("DATA FRAME SENT: \n");
					//~ for(uint32_t i = 0; i < temp_FrameHeader.TotalDataSize/4; i++)
					//~ {
						//~ printf("Tx_FrameData_Byte[%d]: %d\n", i, zart[i]);
					//~ }	
					
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
				
				////////////////////////////////////// OPENING SERIAL PORTS ////////////////////////////////////// 
				//UART HANDLER
				UART_Info.SerialHandle = serOpen(DEFAULT_UART_HANDLER, UART_Info.SerialBaudrate, 0);
				printf("OPENED HANDLER UART: %d\n", UART_Info.SerialHandle);
				if(UART_Info.SerialHandle < 0)
				{
					fprintf(stderr, "UART Serial port initialisation failed\n");
					return ERROR_PIGPIO_UART_INIT;	
				}

				//IF UART TRANSMISSION IS ENABLED
				if((UART_Info.SerialStatus == 2) || (UART_Info.SerialStatus == 3))
				{
					
					if(UART_Info.SerialDataSize > 4)
					{
						if(ClientAvailable)
						{
							//RECEIVING UART DATA FROM PC TO BE SENT on Tx
							if(ClientAvailable)
							UDP_ServerReceive(&sockfd, UART_Info.Tx_Buffer, &cliaddr, (uint32_t *)&len, UART_Info.SerialDataSize);
							for(uint32_t i = 0; i < UART_Info.SerialDataSize ; i++)
							{
								printf("SERIAL_RX[%d]: %02X\n", i, UART_Info.Tx_Buffer[i]);
							}
							//Sending data over UART
							serWrite(UART_Info.SerialHandle, (char *)UART_Info.Tx_Buffer, UART_Info.SerialDataSize);		
						}
						else
						{
							gpio_DeInit();
							break;
						}			
					}
					gpioDelay(100000); //100msec
				}
				
				//IF RECEIVING IS ENABLED
				if((UART_Info.SerialStatus == 1) || (UART_Info.SerialStatus == 3))
				{
					Rx_Counter = 0;
					while(serDataAvailable(UART_Info.SerialHandle) > 0)
					{
						UART_Info.Rx_Buffer[Rx_Counter] = serReadByte(UART_Info.SerialHandle);
						Rx_Counter++;
						printf("Counter: %d\n", Rx_Counter);
					}
					
					for(int i = 0; i < Rx_Counter; i++)
					{
						printf("UART_Rx_BUFFER[%d]: %d\n", i, UART_Info.Rx_Buffer[i]);
					}
					
					UDP_ServerSend(&sockfd, (uint8_t*)&Rx_Counter, &cliaddr, len, sizeof(uint32_t));
					if(Rx_Counter > 0)
					{
						printf("VALUe: %d\n\n", Rx_Counter);
						UDP_ServerSend(&sockfd, (uint8_t*)UART_Info.Rx_Buffer, &cliaddr, len, UART_Info.SerialDataSize);
					}

				}
				if(UART_Info.SerialHandle >= 0)
				{
					printf("CLOSING HANDLER UART: %d\n", UART_Info.SerialHandle);
					serClose(UART_Info.SerialHandle);
					UART_Info.SerialHandle = -1;
				}
				
				/////////////////////////////////	
				
				////// SPI_CH2
				//CHECK WHETHER THE SPI_CH1 is enabled or not
				if((SPI_CH1_Info.SerialStatus == 1) || (SPI_CH1_Info.SerialStatus == 2))
				{
					Rx_Counter = 0;
					printf("FOUND SPI_CH1 ENABLED\n");
					printf("SIZE CHECKING ON SIZE: %d\n", SPI_CH1_Info.SerialDataSize);
					if(SPI_CH1_Info.SerialDataSize > 4)
					{
						printf("FOUND SerialDataSize SPI_CH1 > 4\n");
						if(ClientAvailable)
						{
							printf("FOUND SPI_CH1 CLIENT AVAILABLE\n");
							//OPENING SPI_CH1 HANDLER
							SPI_CH1_Info.SerialHandle = spiOpen(SPI1_CHANNEL, SPI_CH1_Info.SerialBaudrate, SPI1_FLAGS);
							printf("OPENED HANDLER SPI_CH1: %d\n", SPI_CH1_Info.SerialHandle);
							if(SPI_CH1_Info.SerialHandle < 0)
							{
								printf("BAD HANDLER SPI_CH1: %d\n", SPI_CH1_Info.SerialHandle);
								fprintf(stderr, "SPI_CH1 port initialisation failed\n");
								return ERROR_PIGPIO_SPI_CH1_INIT;	
							}
							printf("BEFORE RECEIVING FROM PC TO SPI_CH1\n");
							//RECEIVING SPI_CH1 DATA FROM PC TO BE SENT on SPI
							UDP_ServerReceive(&sockfd, SPI_CH1_Info.Tx_Buffer, &cliaddr, (uint32_t *)&len, SPI_CH1_Info.SerialDataSize);
							for(uint32_t i = 0; i < SPI_CH1_Info.SerialDataSize ; i++)
							{
								printf("SPI_CH1_TX_BUFFER[%d]: %02X\n", i, SPI_CH1_Info.Tx_Buffer[i]);
							}
							//Exchanging data over SPI_CH1
							printf("BEFORE EXCHANGING DATA ON SPI_CH1\n");
							spiXfer(SPI_CH1_Info.SerialHandle, (char *)SPI_CH1_Info.Tx_Buffer, (char *)SPI_CH1_Info.Rx_Buffer, SPI_CH1_Info.SerialDataSize);		
							for(uint32_t i = 0; i < SPI_CH1_Info.SerialDataSize ; i++)
							{
								printf("SPI_CH1_RX_BUFFER[%d]: %02X\n", i, SPI_CH1_Info.Rx_Buffer[i]);
							}
							
							//SENDING THE DATA Received on SPi back to PC
							//Sending the size of Rx data
							printf("BEFORE SENDING SIZE OF RX ON SPI_CH1 BACK TO PC\n");
							
							UDP_ServerSend(&sockfd, (uint8_t*)&SPI_CH1_Info.SerialDataSize, &cliaddr, len, sizeof(uint32_t));
							
							printf("BEFORE SENDING DATA OF RX ON SPI_CH1 BACK TO PC\n");
							//Sending the Rx data
							UDP_ServerSend(&sockfd, (uint8_t*)SPI_CH1_Info.Rx_Buffer, &cliaddr, len, SPI_CH1_Info.SerialDataSize);
				
							//CLOSING THE SPI_CH1 HANDLER
							if(SPI_CH1_Info.SerialHandle >= 0)
							{
								printf("CLOSING HANDLER SPI_CH1: %d\n", SPI_CH1_Info.SerialHandle);
								serClose(SPI_CH1_Info.SerialHandle);
								SPI_CH1_Info.SerialHandle = -1;
							}
						}
						else
						{
							gpio_DeInit();
							break;
						}			
					}
					else
					{
						UDP_ServerSend(&sockfd, (uint8_t*)&Rx_Counter, &cliaddr, len, sizeof(uint32_t));
					}
				}


				/////////////////////////////////////
				////// SPI_CH2
				//CHECK WHETHER THE SPI_CH2 is enabled or not
				if((SPI_CH2_Info.SerialStatus == 1) || (SPI_CH2_Info.SerialStatus == 2))
				{
					Rx_Counter = 0;
					printf("FOUND SPI_CH2 ENABLED\n");
					printf("SIZE CHECKING ON SIZE: %d\n", SPI_CH2_Info.SerialDataSize);
					if(SPI_CH2_Info.SerialDataSize > 4)
					{
						printf("FOUND SerialDataSize SPI_CH2 > 4\n");
						if(ClientAvailable)
						{
							printf("FOUND SPI_CH2 CLIENT AVAILABLE\n");
							//OPENING SPI_CH2 HANDLER
							SPI_CH2_Info.SerialHandle = spiOpen(SPI1_CHANNEL, SPI_CH2_Info.SerialBaudrate, SPI1_FLAGS);
							printf("OPENED HANDLER SPI_CH2: %d\n", SPI_CH2_Info.SerialHandle);
							if(SPI_CH2_Info.SerialHandle < 0)
							{
								printf("BAD HANDLER SPI_CH2: %d\n", SPI_CH2_Info.SerialHandle);
								fprintf(stderr, "SPI_CH2 port initialisation failed\n");
								return ERROR_PIGPIO_SPI_CH2_INIT;	
							}
							printf("BEFORE RECEIVING FROM PC TO SPI_CH2\n");
							//RECEIVING SPI_CH2 DATA FROM PC TO BE SENT on SPI
							UDP_ServerReceive(&sockfd, SPI_CH2_Info.Tx_Buffer, &cliaddr, (uint32_t *)&len, SPI_CH2_Info.SerialDataSize);
							for(uint32_t i = 0; i < SPI_CH2_Info.SerialDataSize ; i++)
							{
								printf("SPI_CH2_TX_BUFFER[%d]: %02X\n", i, SPI_CH2_Info.Tx_Buffer[i]);
							}
							//Exchanging data over SPI_CH2
							printf("BEFORE EXCHANGING DATA ON SPI_CH2\n");
							spiXfer(SPI_CH2_Info.SerialHandle, (char *)SPI_CH2_Info.Tx_Buffer, (char *)SPI_CH2_Info.Rx_Buffer, SPI_CH2_Info.SerialDataSize);		
							for(uint32_t i = 0; i < SPI_CH2_Info.SerialDataSize ; i++)
							{
								printf("SPI_CH2_RX_BUFFER[%d]: %02X\n", i, SPI_CH2_Info.Rx_Buffer[i]);
							}
							
							//SENDING THE DATA Received on SPi back to PC
							//Sending the size of Rx data
							printf("BEFORE SENDING SIZE OF RX ON SPI_CH2 BACK TO PC\n");
							
							UDP_ServerSend(&sockfd, (uint8_t*)&SPI_CH2_Info.SerialDataSize, &cliaddr, len, sizeof(uint32_t));
							
							printf("BEFORE SENDING DATA OF RX ON SPI_CH2 BACK TO PC\n");
							//Sending the Rx data
							UDP_ServerSend(&sockfd, (uint8_t*)SPI_CH2_Info.Rx_Buffer, &cliaddr, len, SPI_CH2_Info.SerialDataSize);
				
							//CLOSING THE SPI_CH2 HANDLER
							if(SPI_CH2_Info.SerialHandle >= 0)
							{
								printf("CLOSING HANDLER SPI_CH2: %d\n", SPI_CH2_Info.SerialHandle);
								serClose(SPI_CH2_Info.SerialHandle);
								SPI_CH2_Info.SerialHandle = -1;
							}
						}
						else
						{
							gpio_DeInit();
							break;
						}			
					}
					else
					{
						UDP_ServerSend(&sockfd, (uint8_t*)&Rx_Counter, &cliaddr, len, sizeof(uint32_t));
					}
				}
			}
		}
	}
	
	/*************************** Disconnecting the UDP connection *********************/
	UDP_ServerDisconnect(&sockfd);

	return 0;
}

