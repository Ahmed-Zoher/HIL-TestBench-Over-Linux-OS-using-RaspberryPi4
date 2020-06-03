#ifndef FRAME_H
#define FRAME_H

/******************** EXAMPLE ********************/
#define DIO_PERIPHERAL_ID		0x76
#define DIO_DATA_SIZE			5

#define ADC_PERIPHERAL_ID		0x45
#define ADC_DATA_SIZE			3

#define SIGNATURE				0x07775000
#define NUM_OF_CMD				2
#define PERIPH_HEADER_SIZE		(8)
#define TOTAL_SIZE				(ADC_DATA_SIZE + DIO_DATA_SIZE + (NUM_OF_CMD * PERIPH_HEADER_SIZE))
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
	uint8_t*  PeripheralData;
}FrameData_t;

uint8_t* FRAME_Generate(void);
void FRAME_Print(uint8_t* Frame);


#endif /* FRAME_H */
