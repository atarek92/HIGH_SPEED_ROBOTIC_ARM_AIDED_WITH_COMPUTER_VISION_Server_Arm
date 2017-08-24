
#include "stdafx.h"
#include "TimerTest.h"
#include "windows.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include <string>


#include<WinSock2.h>
#include<WS2tcpip.h.>


#pragma comment(lib,"Ws2_32.lib")
#define PORT "1234"
#define BUFFERSIZE 512




#define No_of_Bytes 		42
#define Mantissa_Bits		23
#define Exponent_Bits		8
#define Total_Bits			32

using namespace std;

int test=5;
volatile int def = 1, x_cor = 0, y_cor = 0, z_cor = 0, pos=0;

//String for getting the output from arduino
char output[No_of_Bytes];

//Portname must contain these backslashes
char *port_name = "\\\\.\\COM8";

//String for incoming data
char incomingData[No_of_Bytes];

SerialPort arduino(port_name);


int hex_table(char c)
{
	switch (c)
	{
	case '0':
		return 0;
		break;

	case '1':
		return 1;
		break;

	case '2':
		return 2;
		break;

	case '3':
		return 3;
		break;

	case '4':
		return 4;
		break;

	case '5':
		return 5;
		break;

	case '6':
		return 6;
		break;

	case '7':
		return 7;
		break;

	case '8':
		return 8;
		break;

	case '9':
		return 9;
		break;

	case 'a':
		return 10;
		break;

	case 'b':
		return 11;
		break;

	case 'c':
		return 12;
		break;

	case 'd':
		return 13;
		break;

	case 'e':
		return 14;
		break;

	case 'f':
		return 15;
		break;
	}
	return 500;
}

char inverted_ascii(int x)
{

	switch (x)
	{
	case 0:
		return '0';
		break;

	case 1:
		return '1';
		break;

	case 2:
		return '2';
		break;

	case 3:
		return '3';
		break;

	case 4:
		return '4';
		break;

	case 5:
		return '5';
		break;

	case 6:
		return '6';
		break;

	case 7:
		return '7';
		break;

	case 8:
		return '8';
		break;

	case 9:
		return '9';
		break;

	case 10:
		return 'a';
		break;

	case 11:
		return 'b';
		break;

	case 12:
		return 'c';
		break;

	case 13:
		return 'd';
		break;

	case 14:
		return 'e';
		break;

	case 15:
		return 'f';
		break;
	}

	return 'z';
}

int *decimalToBinary(int n)
{
	static int  bin[Mantissa_Bits] = { 0 };
	int remainder = 0, i = 1;

	while ((n != 0))
	{
		remainder = n % 2;
		n = n / 2;
		bin[Mantissa_Bits - i] = remainder;
		i++;
	}

	return bin;
}

int *decimalToBinary2(int n)
{
	static int  bin[Mantissa_Bits] = { 0 };
	int remainder = 0, i = 1;


	while ((n != 0))
	{
		remainder = n % 2;
		n = n / 2;
		bin[Mantissa_Bits - i] = remainder;
		i++;
	}
	return bin;
}

char *decimalToIEEE_754(int decimal)
{
	// the method of conersion is explained in http://teaching.idallen.com/cst8281/10w/notes/100_ieee754_conversions.txt

	static char IEEE_754_Hex[8], IEEE_754_Hex_Little_Endian[8];

	int i = 0, exponent = 0, exponent_127 = 0, sign = 0, x = 0;
	int *binary_ptr = 0, *exponent_bin_pt = 0;
	int binary[Mantissa_Bits] = { 0 }, exponent_bin[Exponent_Bits] = { 0 }, mantissa[Mantissa_Bits] = { 0 }, IEEE_754_bin[Total_Bits] = { 0 };

	if (decimal<0)
	{
		sign = 1;
		decimal = -decimal;
	}
	else
		sign = 0;

	binary_ptr = decimalToBinary(decimal);

	for (int i = 0; i < Mantissa_Bits; i++)
		binary[i] = *(binary_ptr + i);

	for (int i = 0; i < Mantissa_Bits; i++)
		*(binary_ptr + i) = { 0 };


	/***************************Exponent*******************************/

	for (int i = 0; i < Mantissa_Bits; i++)
	if (binary[i] == 1)
	{
		exponent = (Mantissa_Bits - i - 1);
		break;
	}

	exponent_127 = exponent + 127;

	exponent_bin_pt = decimalToBinary2(exponent_127);

	for (int i = 0; i < Exponent_Bits; i++)
		exponent_bin[i] = *(exponent_bin_pt + (Mantissa_Bits - Exponent_Bits) + i);

	for (int i = 0; i < Exponent_Bits; i++)
		*(exponent_bin_pt + (Mantissa_Bits - Exponent_Bits) + i) = { 0 };

	/***************************Mantissa*******************************/

	for (int i = 0; i <Mantissa_Bits; i++)
		mantissa[i] = 0;

	for (int i = Mantissa_Bits - exponent; i <Mantissa_Bits; i++)
		mantissa[i - (Mantissa_Bits - exponent)] = binary[i];

	/***************************IEEE 754*******************************/

	IEEE_754_bin[0] = sign;

	for (int i = 1; i < Exponent_Bits + 1; i++)
		IEEE_754_bin[i] = exponent_bin[i - 1];

	for (int i = Exponent_Bits + 1; i < Total_Bits; i++)
		IEEE_754_bin[i] = mantissa[i - (Exponent_Bits + 1)];

	for (int i = 0; i < Total_Bits; i += 4)
	{
		x = (IEEE_754_bin[i + 3] * 1) + (IEEE_754_bin[i + 2] * 2) + (IEEE_754_bin[i + 1] * 4) + (IEEE_754_bin[i] * 8);
		IEEE_754_Hex[i / 4] = inverted_ascii(x);

	}

	IEEE_754_Hex_Little_Endian[0] = IEEE_754_Hex[6];
	IEEE_754_Hex_Little_Endian[1] = IEEE_754_Hex[7];
	IEEE_754_Hex_Little_Endian[2] = IEEE_754_Hex[4];
	IEEE_754_Hex_Little_Endian[3] = IEEE_754_Hex[5];
	IEEE_754_Hex_Little_Endian[4] = IEEE_754_Hex[2];
	IEEE_754_Hex_Little_Endian[5] = IEEE_754_Hex[3];
	IEEE_754_Hex_Little_Endian[6] = IEEE_754_Hex[0];
	IEEE_754_Hex_Little_Endian[7] = IEEE_754_Hex[1];



	/***************************Printing*******************************/

	//   printf("Binary number of %d is: ", decimal );
	//   for ( int i = 0; i <Mantissa_Bits; i++ )
	//   		printf("%d",binary[i] );
	//
	//   printf("\nSign %d", sign );
	//
	//   printf("\nExponent of %d is: ", exponent );
	//   for ( int i = 0; i <Exponent_Bits; i++ )
	//    	printf("%d",exponent_bin[i] );
	//
	//   printf("\nMaintessa: ");
	//   for ( int i = 0; i <Mantissa_Bits; i++ )
	//    	printf("%d",mantissa[i] );
	//
	//   printf("\nIEEE_754_bin: ");
	//   for ( int i = 0; i <Total_Bits; i++ )
	//    	printf("%d",IEEE_754_bin[i] );
	//
	//   printf("\nIEEE_754_Hex: ");
	//   for ( int i = 0; i <8; i++ )
	//    	printf("%c",IEEE_754_Hex[i] );


	return IEEE_754_Hex_Little_Endian;
}

int hex_to_int(char c, char d)
{
	int high = hex_table(c) * 16;
	int low = hex_table(d);
	return high + low;
}

void CTimerTest::OnTimedEvent1()
{
	int x, j,i, decimal;
	char z, arr[No_of_Bytes * 2];
	char Z_Coordinate_IEEE_754[8], *IEEE_754_Ptr;
	char total_recv[No_of_Bytes] = { 0 };



	if (arduino.isConnected()) 
	{

			printf("Connection Established\n");

			printf("\r\nTimer  Called (count=%i)", timer1.GetCount());

		
			std::string Start_byte = "a5";
			std::string Float_1 = "00004040";				// Mode: Target Movement (Linear)
			std::string Float_2 = "00000000";				// Reserved
			std::string Float_3 = "00008743";				// X-Coordinate
			std::string Float_4 = "00000000";				// Y-Coordinate
			std::string Float_5 = "00001643";				// Z-Coordinate
			std::string Float_6 = "00000000";				// Rotation Angle
			std::string Float_7 = "00000000";				// Suction Cup
			std::string Float_8 = "0000803f";				// Jmp/Movl/J   
			std::string Float_9 = "00000000";				// Gripper
			std::string Float_10 = "00000000";				// Pause Time
			std::string End_byte = "5a";
			
			std::string total;


				if (x_cor == 0)
			{
			Float_3 = "00000000";
			printf("\nx_Cord=0:  ");
			for (i = 0; i < 8; i++)
			printf("%c", Float_3[i]);
			}

			else
			{

				IEEE_754_Ptr = decimalToIEEE_754(x_cor);

				printf("\nX_Cord:  %d    ", x_cor);

			for (i = 0; i < 8; i++)
			{
			Float_3[i] = *(IEEE_754_Ptr + i);
			printf("%c", Float_3[i]);
			*(IEEE_754_Ptr + i) = { 0 };
			}
			}

			if (y_cor == 0)
			{
			Float_4 = "00000000";
			printf("\nY_Cord=0:  ");
			for (i = 0; i < 8; i++)
			printf("%c", Float_4[i]);
			}

			else
			{

				IEEE_754_Ptr = decimalToIEEE_754(y_cor);

			printf("\nY_Cord:  %d    ", y_cor);

			for (i = 0; i < 8; i++)
			{
			Float_4[i] = *(IEEE_754_Ptr + i);
			printf("%c", Float_4[i]);
			*(IEEE_754_Ptr + i) = { 0 };
			}
			}


			if (z_cor == 0)
			{
			Float_5 = "00000000";
			printf("\nZ_Cord=0  ");
			for (i = 0; i < 8; i++)
			printf("%c", Float_5[i]);
			}

			else
			{
				IEEE_754_Ptr = decimalToIEEE_754(z_cor);

			printf("\nZ_Cord:  %d    ", z_cor);

			for (i = 0; i < 8; i++)
			{
			Float_5[i] = *(IEEE_754_Ptr + i);
			printf("%c", Float_5[i]);
			*(IEEE_754_Ptr + i) = { 0 };
			}
			}
			
				/*
			if (test % 4 == 0)
			{
				IEEE_754_Ptr = decimalToIEEE_754(50);

				printf("\nZ_50:  ");

				for (j = 0; j < 8; j++)
				{
					Float_5[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}
				Float_4 = "00000000";
			}
			else if (test % 4 == 1)
			{
				IEEE_754_Ptr = decimalToIEEE_754(0);

				printf("\nZ_0:  ");

				for (j = 0; j < 8; j++)
				{
					Float_5[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}

				IEEE_754_Ptr = decimalToIEEE_754(300);

				printf("\nY_300:  ");

				for (j = 0; j < 8; j++)
				{
					Float_4[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}

				IEEE_754_Ptr = decimalToIEEE_754(0);

				printf("\nX_0:  ");

				for (j = 0; j < 8; j++)
				{
					Float_3[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}
			}

			else if (test % 4 == 2)
			{
				IEEE_754_Ptr = decimalToIEEE_754(0);

				printf("\nZ_0:  ");

				for (j = 0; j < 8; j++)
				{
					Float_5[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}

				IEEE_754_Ptr = decimalToIEEE_754(200);

				printf("\nY_200:  ");

				for (j = 0; j < 8; j++)
				{
					Float_4[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}

				IEEE_754_Ptr = decimalToIEEE_754(0);

				printf("\nX_0:  ");

				for (j = 0; j < 8; j++)
				{
					Float_3[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}


			}
			else if (test% 4 == 3)
			{
				IEEE_754_Ptr = decimalToIEEE_754(0);

				printf("\nZ_0:  ");

				for (j = 0; j < 8; j++)
				{
					Float_5[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}

				IEEE_754_Ptr = decimalToIEEE_754(150);

				printf("\nY_150:  ");

				for (j = 0; j < 8; j++)
				{
					Float_4[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}

				IEEE_754_Ptr = decimalToIEEE_754(0);

				printf("\nX_0:  ");

				for (j = 0; j < 8; j++)
				{
					Float_3[j] = *(IEEE_754_Ptr + j);
					printf("%c", Float_5[j]);
					*(IEEE_754_Ptr + j) = { 0 };
				}
			}
			*/

			//String Concatenation
			total = Start_byte + Float_1 + Float_2 + Float_3 + Float_4 + Float_5 + Float_6 + Float_7 + Float_8 + Float_9 + Float_10 + End_byte;

			for (int i = 0; i<(No_of_Bytes * 2); i++)
				arr[i] = total[i];


		//Creating a c string
		char *c_string = new char[No_of_Bytes + 1];

		//Adding the delimiter
		c_string[No_of_Bytes] = '\n';

		printf("\n");
		printf("Sent Data:");

		// hex_to_Ascii Conversion
		for (int i = 0; i<(No_of_Bytes * 2); i += 2)
		{
			x = hex_to_int(arr[i], arr[i + 1]);
			z = x;
			c_string[i / 2] = z;
			printf("%c", c_string[i / 2]);
		}

		printf("\n");

		//Getting reply from arduino5
		arduino.readSerialPort(total_recv, No_of_Bytes);

		//Writing string to arduino
		arduino.writeSerialPort(c_string, No_of_Bytes);


		//freeing c_string memory
		delete[] c_string;


		printf("Total_Recv: ");

		for (j = 0; j < No_of_Bytes; j++)
		{
			printf("%c", total_recv[j]);
		}




		printf("\n End");
		
	}
		else cout << "ERROR, check port name\n";

}

void CTimerTest::RunTest()
{

	timer1.SetTimedEvent(this, &CTimerTest::OnTimedEvent1);
	timer1.Start(200); // Start timer 1 every 1s



	struct addrinfo *result = NULL, ref;
	WSADATA wsdata;
	WSAStartup(MAKEWORD(2, 2), &wsdata); // Intialization of Server
	// Setting up address
	ZeroMemory(&ref, sizeof(ref));
	ref.ai_family = AF_INET;
	ref.ai_socktype = SOCK_STREAM;
	ref.ai_protocol = IPPROTO_TCP;
	ref.ai_flags = AI_PASSIVE;
	getaddrinfo("127.78.67.34", PORT, &ref, &result);

	// Listen for the Client
	//init listening socket
	SOCKET lSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(lSocket, result->ai_addr, result->ai_addrlen);
	listen(lSocket, SOMAXCONN);

	//Setting up my client
	SOCKET CSocket;
	CSocket = accept(lSocket, NULL, NULL);

	int status;
	int count = 0;
	char revBuffer[BUFFERSIZE];
	int signZ = 0;
	int signX = 0;
	int signY = 0;
	int temp;
	int z = 0;
	int x = 0;
	int y = 0;
	int frame = 0;
	// 0    1   2  3   4 5678  9 10-11-12-13   14-15-16-17-18
	// S    N   P  N   Z 1111  X 1111          Y 
	int ddd = 0;

	while (1)
	{
		count++;
		memset(&revBuffer[0], 0, sizeof(revBuffer));
		status = recv(CSocket, revBuffer, BUFFERSIZE, 0);

		if (status > 0)
		{

			printf_s(revBuffer);



			z = ((revBuffer[5] - '0') * 1000 + (revBuffer[6] - '0') * 100 + (revBuffer[7] - '0') * 10 + (revBuffer[8] - '0') * 1) - 1000;
			x = ((revBuffer[10] - '0') * 1000 + (revBuffer[11] - '0') * 100 + (revBuffer[12] - '0') * 10 + (revBuffer[13] - '0') * 1) - 1000;
			y = ((revBuffer[15] - '0') * 1000 + (revBuffer[16] - '0') * 100 + (revBuffer[17] - '0') * 10 + (revBuffer[18] - '0') * 1) - 1000;

			if (revBuffer[1] == 'N')
			{
				z = z*-1;
			}
			if (revBuffer[2] == 'N')
			{
				x = x*-1;
			}
			if (revBuffer[3] == 'N')
			{
				y = y*-1;
			}

			x_cor = x;
			z_cor = z;

			pos = (int)y;

			if (pos >= 0 && pos < 10)
				y_cor = 30- pos;
			else if (pos >= 10 && pos < 20)
				y_cor = 40 - pos;
			else if (pos >= 20 && pos < 30)
				y_cor = 50 - pos;
			else if (pos >= 30 && pos < 40)
				y_cor = 60 - pos;
			else if (pos >= 40 && pos < 50)
				y_cor = 70 - pos;
			else if (pos >= 50 && pos < 60)
				y_cor = 80 - pos;
			else if (pos >= 60 && pos < 70)
				y_cor = 90 - pos;
			else if (pos >= 70 && pos < 80)
				y_cor = 100 - pos;

			else if (pos >= 80 && pos < 90)
				y_cor = pos - 60;
			else if (pos >= 90 && pos < 100)
				y_cor = pos - 70;
			else if (pos >= 100 && pos < 110)
				y_cor = pos - 80;
			else if (pos >= 110 && pos < 120)
				y_cor = pos - 90;
			else if (pos >= 120 && pos < 130)
				y_cor = pos - 100;
			else if (pos >= 130 && pos < 140)
				y_cor = pos - 110;
			else if (pos >= 140 && pos < 150)
				y_cor = pos - 120;;


			y_cor *= 10; 
		
			if (pos>=80)
				y_cor *= -1;


			//printf("Z=   %d   , X=    %d   , Y=    %d   \n", z, x, y);
			//frame++;
			//printf("Frame= %d", frame);
			/*
			for (int q = 0; q < 20; q++)
			{
			printf("Index = %d  ", q);
			printf("%c", revBuffer[q]);
			printf("\n");
			}
			*/
			//printf_s(revBuffer);


		}
		else
		{
			break;

		}


	}

	shutdown(CSocket, SD_SEND);
	closesocket(CSocket);
	WSACleanup;


//    getchar();          // Wait for return (stop)
//    timer1.Stop();      // Stop timer 1
}
