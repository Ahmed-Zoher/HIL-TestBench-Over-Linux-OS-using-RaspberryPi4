/**
 *  @file client.c
 *  @author May Alaa
 *  @brief PC client interfaces
 */

#include<stdio.h>
#include<stdint.h>
#include<winsock2.h>

#include "client.h"

#pragma comment(lib,"ws2_32") 	//Winsock Library


char buf[BUFLEN];
char recvBuf[BUFLEN];

uint32_t Socket1;

int main(void)
{
	struct sockaddr_in si_other;
	int slen=sizeof(si_other);
	uint8_t Iterator =0;
	Test_frame * frame = (Test_frame*)buf;
	
	frame->ID=5;
	frame->CMD=0x01;
	frame->Ay7aga=7;
	
	UDP_ClientInit(&si_other);

	for(Iterator = 0; Iterator < NUM_OF_FRAMES; Iterator++)
	{
		printf("Press Enter to send frame \n");
		getchar();
		
		UDP_SendFrame(buf,&si_other,sizeof(si_other));
		printf("Frame sent.\n"); 
		
		UDP_ReceiveACK(&si_other, &slen);
		
	}
	UDP_Disconnect(Socket1);

	return 0;
}

/**
 *  @brief This API shall initialize the PC Client UDP link.
 *  
 *  @param [in] si_other Socket
 *  @return void
 */
void UDP_ServerInit(struct sockaddr_in *si_other)
{
	WSADATA wsa;
	//Initialise winsock
	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Initialization Failed. Error Code : %d\n",WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");
	
	//create socket
	if ( (Socket1=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() Failed. Error Code : %d\n" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	//setup address structure
	memset((char *) si_other, 0, sizeof(struct sockaddr_in ));
	si_other->sin_family = AF_INET;
	si_other->sin_port = htons(PORT);
	si_other->sin_addr.S_un.S_addr = inet_addr(SERVER);
	
}

/**
 *  @brief This API shall send a frame
 *  
 *  @param [in] buffer   Buffer 
 *  @param [in] servaddr Server addresss
 *  @param [in] len      Length of frame being sent
 *  @return void
 */
void UDP_SendFrame(uint8_t * buffer, struct sockaddr_in *servaddr, uint32_t len)
{
		//send the message
		if (sendto(Socket1, buf, sizeof(Test_frame) , 0 , (struct sockaddr *)servaddr, len) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}
}

/**
 *  @brief This API shall receive and acknowledgment.
 *  
 *  @param [in] servaddr Server address
 *  @param [in] len      Length of frame being received
 *  @return void
 */
void UDP_ReceiveACK(struct sockaddr_in *servaddr, uint32_t * len)
{
		//clear the buffer by filling null, it might have previously received data
		memset(recvBuf,0, BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(Socket1, recvBuf, sizeof(ACK_SIZE), 0, (struct sockaddr *)servaddr, len) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("%s\n",recvBuf);
}

/**
 *  @brief This API shall terminate the PC Socket UDP connection
 *  
 *  @param [in] sockfd Socket number
 *  @return void
 */
void UDP_Disconnect(uint32_t socket)
{
	closesocket(socket);
	WSACleanup();
}
