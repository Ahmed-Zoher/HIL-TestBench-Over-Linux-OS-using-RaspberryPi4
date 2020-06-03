#ifndef FRAME_H
#define FRAME_H

/******************** EXAMPLE ********************/
#define DIO_PERIPHERAL_ID		0x01
#define DIO_INPUT_PINS			3


#define UART_PERIPHERAL_ID		0x04

#define SIGNATURE				0x07775000
#define NUM_OF_PERIPH			2
#define PERIPH_HEADER_SIZE		8
#define PERIPH_INFO_SIZE		(PERIPH_HEADER_SIZE * NUM_OF_PERIPH)
/*************************************************/


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

void FRAME_GenerateDataFrame(uint8_t* DIO_Data, uint32_t DIO_DataSize , uint8_t* UART_Data, uint32_t UART_DataSize);
void FRAME_Print(void);
uint8_t FRAME_ParsingDataFrame(void);
void FRAME_FreeData(void);


#endif /* FRAME_H */