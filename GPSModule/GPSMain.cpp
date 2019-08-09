#pragma once
#using <System.dll>
#include <SMObject.h>
#include <SMStruct.h> 
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>
#include <Windows.h>
#include <string.h>

#include "GPS_Head.h"

#define Pack_size 112
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net; 
using namespace System::Text;

int main() {
	SMObject PMObj(_TEXT("ProcessManagemnet"), sizeof(PM));
	SMObject GPSObj(_TEXT("GPS"), sizeof(GPS));
	PM* PMSMPtr = nullptr;
	GPS* GPSPtr = nullptr;
	
	//Request access to memory
	GPSObj.SMAccess();
	PMObj.SMAccess();
	if (PMObj.SMAccessError)
	{
		Console::WriteLine("Shared memory access failed");
		return -2;
	}
	if (GPSObj.SMAccessError)
	{
		Console::WriteLine("Shared memory access failed");
		return -2;
	}

	PMSMPtr = (PM*)PMObj.pData;
	GPSPtr = (GPS*)GPSObj.pData;
	
	array<unsigned char>^ ReadData;
	ReadData = gcnew array<unsigned char>(Pack_size);
	
	//Tcp
	TcpClient^ Client;

	Client = gcnew TcpClient("192.168.1.200", GPSPort);
	if (Client->Connected)
	{
		Console::WriteLine("Connected to GPS");
	}
	else
	{
		Console::WriteLine("Cannot find GPS");
		//return 0;
	}

	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	NetworkStream^ stream = Client->GetStream();

	PMSMPtr->TimeStamp[GPS_time] = 0;// PMSMPtr->TimeStamp[PM_time];
	int i = 0;

	GPS MyGps;
	unsigned char* BytePtr;
	BytePtr = (unsigned char*)(&MyGps);

	while (!PMSMPtr->Shutdown.flags.GPS) 
	{
		PMSMPtr->TimeStamp[GPS_time] = GetTimeStamp(PMSMPtr);
		//Console::WriteLine("GPS Timestamp: {0,10:F3}", PMSMPtr->TimeStamp[GPS_time]);
		//Console::WriteLine("TimeDifference: {0,10:F3}", PMSMPtr->TimeStamp[GPS_time]-PMSMPtr->TimeStamp[PM_time]);
		//std::cout << "i: " << i << std::endl;
		if ((PMSMPtr->TimeStamp[GPS_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime) {
			i++;
			if ((i > 20) && ((PMSMPtr->TimeStamp[GPS_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime)) {
				PMSMPtr->Shutdown.Status = 0xff;
				std::cout << "PM error! Shut down all." << std::endl;
			}
			//std::cout << "PM is alive." << std::endl;
		}

		// get GPS data
		stream->Read(ReadData, 0, ReadData->Length);

		for (int i = 0; i <112; i++)
		{
			BytePtr[i] = ReadData[i];
		}

		std::cout<<std::endl;
		GPSPtr->Height = MyGps.Height;
		GPSPtr->Northing = MyGps.Northing;
		GPSPtr->Easting = MyGps.Easting;

		Console::WriteLine("Northing: {0,12:d3}", GPSPtr->Northing);
		Console::WriteLine("Easting:  {0,12:d3}", GPSPtr->Easting);
		Console::WriteLine("Heighe:   {0,12:d3}m", MyGps.Height);
		Console::WriteLine("CheckSum: {0,12:d3}", MyGps.Checksum);
		
		unsigned long CalculateSum = CalculateBlockCRC32(112-4, BytePtr);
		Console::WriteLine("CRC32Sum: {0,12:d3}", CalculateSum);

		if (CalculateSum == MyGps.Checksum) 
		{
			Console::WriteLine("Read all data，Valid;");
		}
		else
		{
			Console::WriteLine("Reading Error");
		}

		if (_kbhit()) break;
		System::Threading::Thread::Sleep(WAIT_TIME);

	}
	stream->Close();
	Client->Close();


	//Console::ReadKey();
	Console::WriteLine("GPS module terminated normally.");
	//Console::ReadKey();
	return 0;
}


