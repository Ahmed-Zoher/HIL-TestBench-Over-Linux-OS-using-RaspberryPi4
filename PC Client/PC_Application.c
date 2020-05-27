#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "client.h"
#include "Frame.h"


/*************************** GLOBAL VARIABLES ***************************/
uint8_t buf[BUFLEN];
uint8_t recvBuf[ACK_SIZE];
uint32_t Socket1;

uint8_t RxFrameHeaderBuffer[100];
uint8_t RxFrameDataBuffer[100];

void main(void)
{
	/***************** Local Variables definitions *****************/
	struct sockaddr_in si_other;
	uint32_t slen = sizeof(si_other);
	uint16_t Iterator = 0;
	uint8_t *Frame = NULL;
	uint8_t ACK_arr[] = "ACK";
	uint8_t NACK_arr[] = "NACK";
	
	FrameHeader_t FrameHeader = 
	{
		.Signature 		=  SIGNATURE,
		.NumOfCommands 	=  NUM_OF_CMD,
		.TotalDataSize 	=  TOTAL_SIZE
	};
	
	/***************** Initializations *****************/
	UDP_ClientInit(&Socket1, &si_other);
	
	/****************** Frame Generation ***************/
	
	
	/****************** Frame Transmission *************/
	/* Header Frame */
	UDP_ClientSend(&Socket1, (uint8_t *)&FrameHeader, &si_other, slen, sizeof(FrameHeader_t));
	UDP_ClientReceive(&Socket1, recvBuf, &si_other, &slen, ACK_SIZE);

	if(strcmp(recvBuf, "ACK") == 0)
	{
		printf("BENCH-Tmam!\n");
		/* DIO Frame + UZART Frame */
		
		Frame = FRAME_Generate();
		FRAME_Print(Frame);
		UDP_ClientSend(&Socket1, Frame, &si_other, slen, FrameHeader.TotalDataSize);
		
	}
	else
	{
		printf("BENCH-Please!\n");
	}
	
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	UDP_ClientReceive(&Socket1, RxFrameHeaderBuffer, &si_other, &slen, sizeof(FrameHeader_t));
	
	printf("RxHEADER - Signature: %04X\n", ((FrameHeader_t *)RxFrameHeaderBuffer)->Signature);
	printf("RxHEADER - NumOfCommands: %d\n", ((FrameHeader_t *)RxFrameHeaderBuffer)->NumOfCommands);
	printf("RxHEADER - TotalDataSize: %d\n", ((FrameHeader_t *)RxFrameHeaderBuffer)->TotalDataSize);
	
	if(((FrameHeader_t *)RxFrameHeaderBuffer)->Signature == SIGNATURE)
	{
		UDP_ClientSend(&Socket1, ACK_arr, &si_other, slen, ACK_SIZE);
		UDP_ClientReceive(&Socket1, RxFrameDataBuffer, &si_other, &slen, ((FrameHeader_t *)RxFrameHeaderBuffer)->TotalDataSize);
		
		for(Iterator = 0; Iterator < ((FrameHeader_t *)RxFrameHeaderBuffer)->TotalDataSize; Iterator++)
		{
			printf("RxDATA - Byte[%d]: %d\n", Iterator, RxFrameDataBuffer[Iterator]);
		}
		
	}
	else
	{
		printf("Pitch Chill!(PC)\n");
	}
	


	/***************** Socket Disconnect ****************/
	UDP_ClientDisconnect(&Socket1);

}

