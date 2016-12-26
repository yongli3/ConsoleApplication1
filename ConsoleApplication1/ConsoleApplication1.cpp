// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>

static int LastError()
{
	LPVOID lpMsgBuf;
	DWORD errorCode;
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
	return 0;
}

static int ReadUart()
{
	HANDLE hComm = INVALID_HANDLE_VALUE;
	HANDLE hWrite = INVALID_HANDLE_VALUE;
	char WriteName[] = "C:\\proj\\write.bin";
	char  ComPortName[] = "\\\\.\\COM8";
	char readBuffer[4096];
	COMMTIMEOUTS timeouts = { 0 };
	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	BOOL  Status;
	BOOL ret;
	DWORD EndTime = 0;
	DWORD StartTime = 0;
	DWORD numRead = 0;
	DWORD numWrite = 0;

	StartTime = timeGetTime();
	// Read UART to a file
	hComm = CreateFile((ComPortName),                  // Name of the Port to be Opened
		GENERIC_READ | GENERIC_WRITE, // Read/Write Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("\n    Error! - Port %s can't be opened\n", ComPortName);
		LastError();
		goto error;
	}
	else
		printf("\n    Port %s Opened\n ", ComPortName);

	/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

	if (Status == FALSE) {
		printf("\n    Error! in GetCommState()");
		LastError();
		goto error;
	}
	dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

	if (Status == FALSE)
	{
		printf("\n    Error! in Setting DCB Structure");
		goto error;
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

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
		printf("\n\n    Error! in Setting Time Outs");
		goto error;
	}
	else
		printf("\n\n    Setting Serial Port Timeouts Successfull\n");

	hWrite = CreateFile(WriteName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWrite == INVALID_HANDLE_VALUE) {
		LastError();
		goto error;
	}

	while (1)  {
		printf("%d", timeGetTime());
		ret = ReadFile(hComm, readBuffer, sizeof(readBuffer), &numRead, NULL);
		printf("-%d ret=%d %d\n", timeGetTime(), ret, numRead);
		if (!ret) {
			printf("read error!\n");
			break;
		}
		if (ret && (numRead == 0)) {
			printf("EOF\n");
			break;
		}

		if (ret && (numRead > 0)) { // write to UART
			ret = WriteFile(hWrite, readBuffer, numRead, &numWrite, NULL);
			printf("write %d-%d\n", ret, numWrite);
			if (!ret) {
				printf("write fail!\n");
				break;
			}
		}
	}

error:
	StartTime = timeGetTime();
	printf("Time = %dms\n", EndTime - StartTime);
	CloseHandle(hComm);
	CloseHandle(hWrite);
	return ret;
}

int main(int argc, _TCHAR* argv[])
{
	HANDLE hread = INVALID_HANDLE_VALUE;
	HANDLE hwrite = INVALID_HANDLE_VALUE;
	HANDLE hComm = INVALID_HANDLE_VALUE;                          // Handle to the Serial port
	char WriteName[] = "C:\\proj\\write.bin";
	char ReadName[] = "C:\\proj\\read.bin";
	char  ComPortName[] = "\\\\.\\COM8";  // Name of the Serial port(May Change) to be opened,
	BOOL  Status;                          // Status of the various operations 
	DWORD dwEventMask;                     // Event mask to trigger
	char  TempChar;                        // Temperory Character
	char  SerialBuffer[1024];               // Buffer Containing Rxed Data
	char readBuffer[4096];
	char writeBuffer[1024];
	DWORD NoBytesRead;                     // Bytes read by ReadFile()
	int i = 0;
	LPVOID lpMsgBuf;
	DWORD StartTime = 0;
	DWORD EndTime = 0;
	DWORD errorCode;
	DWORD numRead = 0;
	DWORD numWrite = 0;
	BOOL ret;
	unsigned char   writebuffer[1024];		       // lpBuffer should be  char or byte array, otherwise write wil fail
	DWORD  dNoOFBytestoWrite;              // No of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;
	COMMTIMEOUTS timeouts = { 0 };
	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure

	memset(writebuffer, 0, sizeof(writebuffer));
	dNoOFBytestoWrite = sizeof(writebuffer);
	printf("%s\n", __FUNCDNAME__);

	ReadUart();
	return 0;

#if 0
	// Test file read/write 
	StartTime = timeGetTime();
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
		goto error;
	}

	hwrite = CreateFile(WriteName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hwrite == INVALID_HANDLE_VALUE) {
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
		printf("Write file fail! [%s]\n", (char *)lpMsgBuf);
		LocalFree(lpMsgBuf);
		goto error;
	}

	while (1) {
		ret = ReadFile(hread, readBuffer, sizeof(readBuffer), &numRead, NULL);
		printf("read %d %d\n", ret, numRead);
		if (!ret) {
			printf("read error!\n");
			break;
		}
		if (ret && (numRead == 0)) {
			printf("EOF\n");
			break;
		}

		if (ret && (numRead > 0)) { // write to new file
			ret = WriteFile(hwrite, readBuffer, numRead, &numWrite, NULL);
			printf("write %d-%d\n", ret, numWrite);
			if (!ret) {
				printf("write fail!\n");
				break;
			}
		}
	}

	CloseHandle(hread);
	CloseHandle(hwrite);
	EndTime = timeGetTime();
	printf("Use %dms\n", EndTime - StartTime);
	return 0;
#endif

	StartTime = timeGetTime();
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
		goto error;
	}

	// write file to UART
	hComm = CreateFile((ComPortName),                  // Name of the Port to be Opened
		GENERIC_READ | GENERIC_WRITE, // Read/Write Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("\n    Error! - Port %s can't be opened\n", ComPortName);
		LastError();
		goto error;
	}
	else
		printf("\n    Port %s Opened\n ", ComPortName);

	/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

	if (Status == FALSE) {
		printf("\n    Error! in GetCommState()");
		LastError();
		goto error;
	}
	dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

	if (Status == FALSE)
	{
		printf("\n    Error! in Setting DCB Structure");
		goto error;
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

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(hComm, &timeouts) == FALSE) {
		printf("\n\n    Error! in Setting Time Outs");
		goto error;
	}
	else
		printf("\n\n    Setting Serial Port Timeouts Successfull\n");

	// write to COM
	for (i = 0; i < sizeof(writebuffer) - 1; i++) {
		writebuffer[i] = i;
	}

	while (1) { // read file and write to UART
		ret = ReadFile(hread, readBuffer, sizeof(readBuffer), &numRead, NULL);
		printf("read %d %d\n", ret, numRead);
		if (!ret) {
			printf("read error!\n");
			break;
		}
		if (ret && (numRead == 0)) {
			printf("EOF\n");
			break;
		}

		if (ret && (numRead > 0)) { // write to UART
			ret = WriteFile(hComm, readBuffer, numRead, &numWrite, NULL);
			printf("write %d-%d\n", ret, numWrite);
			if (!ret) {
				printf("write fail!\n");
				break;
			}
		}
	}

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

error:
	CloseHandle(hread);
	CloseHandle(hwrite);
	CloseHandle(hComm);//Closing the Serial Port
	return ret;
}

