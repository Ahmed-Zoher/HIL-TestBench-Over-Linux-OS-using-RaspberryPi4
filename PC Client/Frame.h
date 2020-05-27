#ifndef FRAME_H
#define FRAME_H


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

uint8_t* FRAME_Generate(void);



#endif /* FRAME_H */