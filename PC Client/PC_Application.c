#include "client.h"
#include "Frame.h"


/*************************** GLOBAL VARIABLES ***************************/
uint8_t buf[BUFLEN];
uint8_t recvBuf[BUFLEN];
uint32_t Socket1;


void main(void)
{
	/***************** Local Variables definitions *****************/
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	uint8_t Iterator =0;
	Test_frame * frame = (Test_frame*)buf;
	frame->ID=5;
	frame->CMD=0x01;
	frame->Ay7aga=7;
	
	/***************** Initializations *****************/
	UDP_ClientInit(&Socket1, &si_other);
	
	
	/****************** Frame Generation ***************/
	
	
	
	
	
	/****************** Frame Transmission *************/
	for(Iterator = 0; Iterator < NUM_OF_FRAMES; Iterator++)
	{
		printf("Press Enter to send frame \n");
		getchar();
		
		UDP_ClientSendFrame(&Socket1, buf,&si_other,sizeof(si_other));
		printf("Frame sent.\n"); 
		
		UDP_ClientReceiveACK(&Socket1, &si_other, &slen);
	}

	/***************** Socket Disconnect ****************/
	UDP_ClientDisconnect(&Socket1);
	
}

