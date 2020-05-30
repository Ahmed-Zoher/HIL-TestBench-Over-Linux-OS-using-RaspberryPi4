#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "client.h"
#include "Frame.h"


/*************************** GLOBAL VARIABLES ***************************/

	
void main(void)
{
	/***************** Local Variables definitions *****************/

	
	
	
	/***************** Initializations *****************/
	//UDP_ClientConnect(&Socket1, &si_other);
	UDP_ClientConnect();
	
	UDP_ClientSend(&Socket1, (uint8_t *)&FrameHeader, &si_other, slen, sizeof(FrameHeader_t));
	//UDP_ClientSend();
	
	UDP_ClientReceive(&Socket1, recvBuf, &si_other, &slen, ACK_SIZE);
	//UDP_ClientReceive();

	if(strcmp(recvBuf, "ACK") == 0)
	{
		printf("BENCH-Tmam!\n");
		/* DIO Frame + UZART Frame */
		
		Frame = FRAME_Generate();
		FRAME_Print(Frame);
		//UDP_ClientSend(&Socket1, Frame, &si_other, slen, FrameHeader.TotalDataSize);
		
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

