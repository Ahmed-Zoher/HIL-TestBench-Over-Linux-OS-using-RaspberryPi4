#include "Frame.h"
#include <stdlib.h>

typedef struct{
	uint32_t Signature;
	uint16_t NumOfCommands;
	uint16_t TotalDataSize;
}FrameHeader_t;


typedef struct{
	uint32_t  PeripheralID;
	uint32_t  DataSize;
	uint8_t*  PeripheralData;
}FrameData_t;

/************************** GLOBAL VARIABLES **************************/
uint8_t FrameData[TotalDataSize] = {0};


uint8_t* FRAME_Generate(void)
{
	
	//uint8_t *Frame = (uint8_t *) malloc((Object->TotalDataSize) * sizeof(uint8_t));
	
	
	return Frame;
}