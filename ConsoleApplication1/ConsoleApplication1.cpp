// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>

int main(int argc, _TCHAR* argv[])
{
	HANDLE hread;
	HANDLE hwrite;
	HANDLE hComm;                          // Handle to the Serial port
	char WriteName[] = "C:\\proj\\write.bin";
	char ReadName[] = "C:\\proj\\read.bin";
	char  ComPortName[] = "\\\\.\\COM8";  // Name of the Serial port(May Change) to be opened,
	BOOL  Status;                          // Status of the various operations 
	DWORD dwEventMask;                     // Event mask to trigger
	char  TempChar;                        // Temperory Character
	char  SerialBuffer[1024];               // Buffer Containing Rxed Data
	char readBuffer[1024];
	char writeBuffer[1024];
	DWORD NoBytesRead;                     // Bytes read by ReadFile()
	int i = 0;
	LPVOID lpMsgBuf;
	DWORD errorCode;
	DWORD num = 0;
	BOOL ret;
	printf("%s\n", __FUNCDNAME__);

	hread = CreateFile(ReadName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hread == INVALID_HANDLE_VALUE) {
		errorCode = GetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		printf("Read file fail! [%s]\n", (char *)lpMsgBuf);
		LocalFree(lpMsgBuf);
		return -1;
	}

	while (1) {
		ret = ReadFile(hread, readBuffer, sizeof(readBuffer), &num, NULL);
		if (ret && (num == 0)) {
			printf("EOF\n");
			break;
		}
	}


	CloseHandle(hread);

	hComm = CreateFile((ComPortName),                  // Name of the Port to be Opened
		GENERIC_READ | GENERIC_WRITE, // Read/Write Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
		printf("\n    Error! - Port %s can't be opened\n", ComPortName);
	else
		printf("\n    Port %s Opened\n ", ComPortName);

	/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

	if (Status == FALSE)
		printf("\n    Error! in GetCommState()");

	dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

	if (Status == FALSE)
	{
		printf("\n    Error! in Setting DCB Structure");
	}
	else //If Successfull display the contents of the DCB Structure
	{
		printf("\n\n    Setting DCB Structure Successfull\n");
		printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
		printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
		printf("\n       StopBits = %d", dcbSerialParams.StopBits);
		printf("\n       Parity   = %d", dcbSerialParams.Parity);
	}

	/*------------------------------------ Setting Timeouts --------------------------------------------------*/

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(hComm, &timeouts) == FALSE)
		printf("\n\n    Error! in Setting Time Outs");
	else
		printf("\n\n    Setting Serial Port Timeouts Successfull");

	// write to COM
	unsigned char   writebuffer[1024];		       // lpBuffer should be  char or byte array, otherwise write wil fail
	DWORD  dNoOFBytestoWrite;              // No of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;

	memset(writebuffer, 0, sizeof(writebuffer));
	dNoOFBytestoWrite = sizeof(writebuffer);

	for (i = 0; i < sizeof(writebuffer) - 1; i++) {
		writebuffer[i] = i;
	}

	Status = WriteFile(hComm,               // Handle to the Serialport
		writebuffer,            // Data to be written to the port 
		dNoOFBytestoWrite,   // No of bytes to write into the port
		&dNoOfBytesWritten,  // No of bytes written to the port
		NULL);

	if (Status == TRUE)
		printf("\n\n    %s - Written to %s", writebuffer, ComPortName);
	else
		printf("\n\n   Error %d in Writing to Serial Port",GetLastError());


#if 0
	/*------------------------------------ Setting Receive Mask ----------------------------------------------*/

	Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

	if (Status == FALSE)
		printf("\n\n    Error! in Setting CommMask");
	else
		printf("\n\n    Setting CommMask successfull");


	/*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

	printf("\n\n    Waiting for Data Reception");

	Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

	/*-------------------------- Program will Wait here till a Character is received ------------------------*/

	if (Status == FALSE)
	{
		printf("\n    Error! in Setting WaitCommEvent()");
	}
	else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
	{
		printf("\n\n    Characters Received");
		do
		{
			Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
			SerialBuffer[i] = TempChar;
			i++;
		} while (NoBytesRead > 0);



		/*------------Printing the RXed String to Console----------------------*/

		printf("\n\n    ");
		int j = 0;
		for (j = 0; j < i - 1; j++)		// j < i-1 to remove the dupliated last character
			printf("%c", SerialBuffer[j]);

	}
#endif

	CloseHandle(hComm);//Closing the Serial Port
	return 0;
}

