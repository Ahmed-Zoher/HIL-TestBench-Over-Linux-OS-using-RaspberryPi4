#ifndef CLIENT_H
#define CLIENT_H


#include <winsock2.h>
#pragma comment(lib,"ws2_32") 	//Winsock Library

/************* MACROS DEFINITIONS ************/
#define SERVER 				"192.168.5.10"	//ip address of udp server
#define BUFLEN 				512				//Max length of buffer
#define PORT 				8888			//The port on which to listen for incoming data
#define ACK					0x05
#define NACK				0x08
#define STATUS_SIZE			1
#define NUM_OF_FRAMES		3
#define CONNECTION_KEY		0x12487

#define CONNECTION_OK					0
#define CONNECTION_WINSOCK_INIT_ERROR	1
#define CONNECTION_SOCKET_ERROR			2
#define CONENCTION_REQUEST_TIMEOUT		3

#define HEADER_VALID		0
#define HEADER_INVALID		1
/************* MESSAGE TYPES MACROS ************/
#define	MESSAGE_ACK				0
#define MESSAGE_NACK			1
#define MESSAGE_HEADER_FRAME	2
#define MESSAGE_DATA_FRAME		3
#define MESSAGE_CONNECTION_KEY	4
#define MESSAGE_UART			5
#define MESSAGE_SPI_CH1			6
#define MESSAGE_SPI_CH2			7
#define MESSAGE_SERIAL_SIZE		8

/********** PC CLIENT INTERFACES ***********/

uint8_t UDP_ClientConnect(uint8_t* ServerIP, uint16_t ServerPort);

void UDP_ClientSend(uint8_t MessageType);

uint8_t UDP_ClientReceive(uint8_t MessageType);

void UDP_ClientDisconnect(void);


#endif /* CLIENT_H */