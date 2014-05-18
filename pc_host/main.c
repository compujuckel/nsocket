#include <navnet.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>

volatile int wait;
volatile nn_ch_t conn;
int connected = 0;

static void notifycb(void) {
	wait = 1;
}

static void nsocket_callback(nn_ch_t ch, void* data) {
	conn = ch;
	SOCKET s = *(SOCKET*)data;
	unsigned buf_size = TI_NN_GetConnMaxPktSize(ch);
	char* buf = malloc(buf_size);
	if (!buf)
	{
		puts("Could not alloc buf\n");
		return;
	}
	
	unsigned data_size;
	if (TI_NN_Read(ch, 10000, buf, buf_size - 1, &data_size) < 0) {
		puts("nsocket_callback: error in TI_NN_Read()");
		fflush(stdout);
		return;
	}
	((char*)buf)[data_size] = '\0';
	printf("Received: '%s'\n", (char*)buf);
	fflush(stdout);
	
	if(!connected)
	{
		int status;
		int delimiter = strcspn(buf, ":");
		short port = (short)strtol(buf + delimiter + 1, NULL, 0);
		buf[delimiter] = 0;
		
		printf("Host: %s, Port: %d\n",buf,port);
		
		struct sockaddr_in server;
		struct hostent* host_info;
		unsigned long addr;
		
		memset(&server, 0, sizeof(server));
		if((addr = inet_addr(buf)) != INADDR_NONE)
		{
			memcpy((char*)&server.sin_addr, &addr, sizeof(addr));
		}
		else
		{
			host_info = gethostbyname(buf);
			if(host_info == NULL)
			{
				puts("Could not resolve hostname");
				status = -1;
				TI_NN_Write(ch, &status, sizeof(status));
				return;
			}
			
			memcpy((char*)&server.sin_addr, host_info->h_addr, host_info->h_length);
		}
		
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		
		if(connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
		{
			puts("Could not connect");
			status = -2;
			TI_NN_Write(ch, &status, sizeof(status));
			return;
		}
		
		connected = 1;
		status = 0;
		TI_NN_Write(ch, &status, sizeof(status));
	}
}

int main(void) {
	// Init Winsock
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(1, 1);
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		puts("WSAStartup failed");
		return 1;
	}
	
	// Init NavNet
	if (TI_NN_Init("-c 1 -d 1") < 0)
	{
		puts("NN init failed");
		return 1;
	}
	
	// Wait till a device is connected
	wait = 0;
	TI_NN_RegisterNotifyCallback(0, notifycb);
	while(!wait);
	
	puts("Device connected.");
	fflush(stdout);
	
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if(s < 0)
	{
		puts("Creating socket failed");
		return 1;
	}
	
	TI_NN_StartService(0x8001, &s, &nsocket_callback);
	
	while(!connected);
	
	unsigned long mode = 1;
	ioctlsocket(s,FIONBIO,&mode);
	
	unsigned int bufsize = TI_NN_GetConnMaxPktSize(conn);
	
	puts("Listening...");
	while(1)
	{
		// Get data from socket
		char buf[bufsize];
		int size = recv(s, buf, bufsize, 0);
		if(size > 0)
		{
			if(TI_NN_Write(conn, buf, size) < 0)
				break;
		}
		
		// Get data from calc
		char buf2[200] = {0};
		uint32_t recv_size;
		int ret = TI_NN_Read(conn, 300, buf2, sizeof(buf), &recv_size);
		if(ret < 0)
			break;
		if(recv_size > 0)
		{
			send(s, buf2, recv_size, 0);
			//printf("%s\n",buf2);
		}
		
		Sleep(100);
	}
	
	closesocket(s);
	WSACleanup();
	TI_NN_StopService(0x8001);
	TI_NN_Shutdown();
	return 0;
}