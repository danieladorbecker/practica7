#define _GNU_SOURCE
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>

#define PORT2 8088
#define bufferLen 100

int SensFrmCount = 0;

typedef struct
{
	uint8_t SOF;
	char SensorFrame[4];
	uint8_t Data;
	uint8_t Checksum;
}frame1;

frame1 FrameToSend;
frame1 FTS;

uint8_t HexChecksum(frame1 *DataCRC)
{
	uint8_t ChkSum = 1;
	ChkSum = DataCRC->SOF ^ DataCRC->SensorFrame[0];
	printf("Checksum: %x\n", ChkSum);
	ChkSum = ChkSum ^ DataCRC->SensorFrame[1];
	printf("Checksum: %x\n", ChkSum);
	ChkSum = ChkSum ^ DataCRC->SensorFrame[2];
	printf("Checksum: %x\n", ChkSum);
	ChkSum = ChkSum ^ DataCRC->SensorFrame[3];
	printf("Checksum: %x\n", ChkSum);
	ChkSum = ChkSum ^ DataCRC->Data;
	printf("Checksum: %x\n", ChkSum);
	
	printf("Data Frame \nSOF: %x \nSensor Frame: %x, %x, %x, %x \nData: %x\n", DataCRC->SOF, DataCRC->SensorFrame[0], DataCRC->SensorFrame[1], DataCRC->SensorFrame[2], DataCRC->SensorFrame[3], DataCRC->Data);
	
	return ChkSum;
}

frame1 SensorFrameCreate(uint8_t frameData)
{
	

	//Defining frame1 values
    FrameToSend.SOF = 0x15;	//0x15
   	FrameToSend.SensorFrame[0] = 0x54;	//TIVA
   	FrameToSend.SensorFrame[1] = 0x49;	//TIVA
   	FrameToSend.SensorFrame[2] = 0x56;	//TIVA
   	FrameToSend.SensorFrame[3] = 0x41;	//TIVA
   	FrameToSend.Data = frameData;	//User provided data
   	FrameToSend.Checksum = HexChecksum(&FrameToSend);
   	printf("Checksum: %x\n", FrameToSend.Checksum);
    
    return FrameToSend;
}


int main(int argc, char  *argv[])
{
    int32_t html_server_fd; 
    int32_t new_html_socket; 
    struct sockaddr_in address;
    int32_t option = 1;
    int32_t addresslen = sizeof(address);
    int value = 3;
    int32_t x= 33;
    
    char *line0 = "SOF: \r\n\r\n";
    char *line1 = "Device Address: \r\n\r\n";
    char *line2 = "Data: \r\n\r\n";
    char *line3 = "Checksum: \r\n\r\n";
    char *line4 = "TPS Opening Percentage: \r\n\r\n";
    char sendBuffer[bufferLen];
    char *num;

	FTS = SensorFrameCreate(value);

    // Creating socket file descriptor
    if ((html_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    // Forcefully attaching socket to the port 8080
    if (setsockopt(html_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &option, sizeof(option)))
    {
        perror("setsockopt");
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT2 );
       
    
    if (bind(html_server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        return 1;
    }
    if ((x = (listen(html_server_fd, 3)) < 0))
    {
        perror("listen");
        return -1;
    }
    if ((new_html_socket = accept(html_server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addresslen))<0)
    {
        perror("accept");
        return 1;
    }

    if(x == 0)
    {
        line0 = "SOF: 0x";
        asprintf(&num, "%x", FTS.SOF);
        strcat(strcpy(sendBuffer, line0), num);
        send(new_html_socket , sendBuffer , strlen(sendBuffer) , 0 );
        
        line1 = "\nDevice Address: ";
        asprintf(&num, "%s", FTS.SensorFrame);
        strcat(strcpy(sendBuffer, line1), num);
        send(new_html_socket , sendBuffer , strlen(sendBuffer) , 0 );
        
        line2 = "\nData: 0x";
        asprintf(&num, "%x", FTS.Data);
        strcat(strcpy(sendBuffer, line2), num);
        send(new_html_socket , sendBuffer , strlen(sendBuffer) , 0 );
        
        line3 = "\nChecksum: 0x";
        asprintf(&num, "%x", FTS.Checksum);
        strcat(strcpy(sendBuffer, line3), num);
        send(new_html_socket , sendBuffer , strlen(sendBuffer) , 0 );
        
        line4 = "\n\nTPS Opening Percentage: ";
        asprintf(&num, "%d", value);
        strcat(strcpy(sendBuffer, line4), num);
        send(new_html_socket , sendBuffer , strlen(sendBuffer) , 0 );
        x = 1;
    }
    if ((x = (listen(html_server_fd, 1)) < 0))
    {
        perror("listen");
        return -1;
    }
    printf("Hello message sent\n");
    return 0;
}
