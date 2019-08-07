#pragma once
#using <System.dll>

#define CRC32_POLYNOMIAL			0xEDB88320L
#include <conio.h>
#pragma pack(1)

#define NO_ERROR 0
#define PORT_OPEN_ERROR 2
#define READ_TIMEOUT 3
#define NO_HEADER 4
#define CRC_ERROR 5

using namespace System;
using namespace System::IO::Ports;

// Get error function declaration
void GerErrorDescription(int errorCode);

//Global data
double Northing;
double Easting;
double Height;
int ErrorCode = 0;
GPS NovatelGPS;

unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}
/* --------------------------------------------------------------------------
Calculates the CRC-32 of a block of data all at once
-------------------------------------------------------------------------- */
unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}

//Event handler definition
void OnDataReceived(System::Object^ sender, System::IO::Ports::SerialDataReceivedEventArgs^ e)
{
	// Do the GPS calculation here and find Northing, Esting and Height.
	unsigned int CalculatedCRC;
	int DataReadTimeout = 0;
	SerialPort^ ePort = (SerialPort^)sender;
	unsigned char Buffer[sizeof(GPS)];
	array<unsigned char>^ RecvData = gcnew array<unsigned char>(128);

	ErrorCode = NO_ERROR;

	while (ePort->BytesToRead < sizeof(GPS))
	{
		System::Threading::Thread::Sleep(2);
		// put a time out condition
		if (++DataReadTimeout > 2)
		{
			ErrorCode = READ_TIMEOUT;
			return;
		}

	}
	ePort->Read(RecvData, 0, sizeof(GPS));
	//check the frist 4 headers
	if (*(unsigned int*)(&RecvData) != 0x1c1244aa)
	{
		ErrorCode = NO_HEADER;
		return;
	}
	unsigned char* BytePtr = (unsigned char*)& NovatelGPS;
	for (int i = 0; i < sizeof(GPS); i++)
	{
		*(BytePtr + i) = RecvData[i];
		Buffer[i] = RecvData[i];
	}
		
	CalculatedCRC = CalculateBlockCRC32(108, Buffer);

	if (CalculatedCRC == NovatelGPS.Checksum)
	{
		Height = NovatelGPS.Height;
		Northing = NovatelGPS.Northing;
		Easting = NovatelGPS.Easting;
	}
	else
	{
		ErrorCode = CRC_ERROR;
		return;
	}
}

void GerErrorDescription(int errorCode)
{
	switch (errorCode)
	{
	case PORT_OPEN_ERROR: Console::WriteLine("Port opening failed");
		break;
	case READ_TIMEOUT: Console::WriteLine("Data read time out in handler.");
		break;
	case NO_HEADER: Console::WriteLine("GPS Sync bytes not found in data.");
		break;
	case CRC_ERROR: Console::WriteLine("Checksum error in GPS data.");
		break;
	default: Console::WriteLine("Unrecognized error code.");
	}
}