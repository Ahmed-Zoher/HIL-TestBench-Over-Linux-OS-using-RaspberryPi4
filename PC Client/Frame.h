#ifndef FRAME_H
#define FRAME_H

/******************** EXAMPLE ********************/
#define DIO_PERIPHERAL_ID		0x01
#define DIO_INPUT_PINS			3

#define PWM_PERIPHERAL_ID		0x04
#define PWM_CONFIG_SIZE			16

#define UART_PERIPHERAL_ID		0x07
#define UART_CONFIG_SIZE		12

#define SPI_CH1_PERIPHERAL_ID	0x08
#define SPI_CH1_CONFIG_SIZE		12

#define SPI_CH2_PERIPHERAL_ID	0x09
#define SPI_CH2_CONFIG_SIZE		12

#define SIGNATURE				0x07775000
#define NUM_OF_PERIPH			5
#define PERIPH_HEADER_SIZE		8
#define PERIPH_ID_SIZE			(sizeof(uint32_t))
#define PERIPH_INFO_SIZE		(PERIPH_HEADER_SIZE * NUM_OF_PERIPH)
/*************************************************/

#define SERIAL_UART				0
#define SERIAL_SPI_CH1			1
#define SERIAL_SPI_CH2			2

typedef struct
{
	uint32_t Signature;
	uint16_t NumOfCommands;
	uint16_t TotalDataSize;
}FrameHeader_t;

typedef struct
{
	uint32_t  PeripheralID;
	uint32_t  DataSize;
	uint8_t   PeripheralData;
}FrameData_t;

void FRAME_GenerateDataFrame(uint8_t* DIO_Data, uint8_t* PWM_Config, uint8_t* UART_Config, uint8_t* SPI_CH1_Config, uint8_t* SPI_CH2_Config);
void FRAME_SerialFrameGenerate(uint8_t *Serial_Data, uint32_t Serial_DataSize, uint8_t SerialIndex);
uint8_t *FRAME_ReturnSerial(void);
void FRAME_ReturnSerialFree(void);
void FRAME_Print(void);
uint8_t FRAME_ParsingDataFrame(void);
void FRAME_FreeData(void);


#endif /* FRAME_H */