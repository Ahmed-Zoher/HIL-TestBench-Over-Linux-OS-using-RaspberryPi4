#ifndef CLIENT_H
#define CLIENT_H


#include <winsock2.h>
#pragma comment(lib,"ws2_32") 	//Winsock Library

/************* MACROS DEFINITIONS ************/
#define SERVER 				"192.168.5.10"	//ip address of udp server
#define BUFLEN 				512				//Max length of buffer
#define PORT 				8080			//The port on which to listen for incoming data
#define ACK_SIZE 			4
#define NACK_SIZE 			5
#define NUM_OF_FRAMES		3

#define CONNECTION_OK					0
#define CONNECTION_WINSOCK_INIT_ERROR	1
#define CONNECTION_SOCKET_ERROR			2


/********** PC CLIENT INTERFACES ***********/


uint8_t UDP_ClientConnect(void);

void UDP_ClientSend(uint8_t* buffer, uint32_t FrameSize);

void UDP_ClientReceive(uint8_t* buffer, uint32_t FrameSize);

void UDP_ClientDisconnect(void);


#endif /* CLIENT_H */