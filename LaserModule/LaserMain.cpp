#using <System.dll>

#include<SMObject.h>
#include<SMStruct.h>
#include <conio.h>//_kbhit()
#include <iostream>
#include <tlhelp32.h>
#include <Windows.h>
#include <string.h>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int main()
{
	SMObject PMObj(_TEXT("ProcessManagemnet"), sizeof(PM));
	SMObject LaserObj(_TEXT("LaserData"), sizeof(Laser));
	PM* PMSMPtr = nullptr;
	Laser* LaserPtr = nullptr;

	PMObj.SMAccess();
	LaserObj.SMAccess();
	LaserPtr = (Laser*)LaserObj.pData;
	PMSMPtr = (PM*)PMObj.pData;

	// LMS151 port number must be 23000
	int PortNumber = 23000;

	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;

	// arrays of unsigned chars to send and receive data
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	array<unsigned char>^ SendAuthor;

	String^ AskScan = gcnew String("sRN LMDscandata");
	String^ Author = gcnew String("5125710\n");

	// String to store received data for display
	String^ ResponseData;

	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	if (Client->Connected)
	{
		Console::WriteLine("Connected to LIDAR");
	}
	else
	{
		Console::WriteLine("Cannot find LIDAR");
		//return 0;
	}

	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 2048;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);

	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	SendAuthor = System::Text::Encoding::ASCII->GetBytes(Author);

	array<double>^ Range = nullptr;
	array<double>^ RangeX = nullptr;
	array<double>^ RangeY = nullptr;
	double StartAngle;
	double Resolution;
	int NumRanges;
	array<String^>^ Fragments = nullptr;

	// Get the network streab object associated with clien so we 
	// can use it to read and write
	NetworkStream^ Stream = Client->GetStream();

	//send ID to LiDAR
	Stream->Write(SendAuthor, 0, SendAuthor->Length);
	System::Threading::Thread::Sleep(10);
	Stream->Read(ReadData, 0, ReadData->Length);

	//loop
	PMSMPtr->TimeStamp[Laser_time] = 0;// PMSMPtr->TimeStamp[PM_time] + 1;
	PMSMPtr->Shutdown.flags.Laser = 0;
	int i = 0;
	while (!PMSMPtr->Shutdown.flags.Laser) {
		PMSMPtr->TimeStamp[Laser_time] = GetTimeStamp(PMSMPtr);
		////Console::WriteLine("Laser Timestamp: {0,10:F3}", PMSMPtr->TimeStamp[Laser_time]);
		////Console::WriteLine("TimeDifference: {0,10:F3}", PMSMPtr->TimeStamp[Laser_time] - PMSMPtr->TimeStamp[PM_time]);
		////std::cout << "i: " << i << std::endl;
		//if ((PMSMPtr->TimeStamp[Laser_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime) {
		//	i++;
		//	if ((i > 20) && ((PMSMPtr->TimeStamp[Laser_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime)) {
		//		PMSMPtr->Shutdown.Status = 0xff;
		//		std::cout << "PM error! Shut down all." << std::endl;
		//	}
		//	//std::cout << "PM is alive." << std::endl;
		//}

		// Write command asking for data
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);

		// Read the incoming data
		Stream->Read(ReadData, 0, ReadData->Length);

		// Convert incoming data from an array of unsigned char bytes to an ASCII string
		ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
		
		Fragments  =  ResponseData->Split(' ');
		
		std::cout <<"Length: "<< Fragments->Length << std::endl;

		StartAngle = System::Convert::ToInt32(Fragments[23], 16);
		Resolution = System::Convert::ToInt32(Fragments[24], 16) / 10000.0;
		NumRanges = System::Convert::ToInt32(Fragments[25], 16);
		LaserPtr->Resolution = Resolution;
		LaserPtr->NumPoints = NumRanges;
		LaserPtr->startAngle = StartAngle;

		std::cout << "Start Angle: " << StartAngle << " Resolution: " << Resolution << " Number of Ranges: " << NumRanges << std::endl;

		Range = gcnew array<double>(NumRanges);
		RangeX = gcnew array<double>(NumRanges);
		RangeY = gcnew array<double>(NumRanges);

		//if(Fragments->Length > 500) {
		for (int i = 0; i < NumRanges; i++)
		{
			Range[i] = System::Convert::ToInt32(Fragments[26 + i], 16);

			RangeX[i] = Range[i] * Math::Sin(i * Resolution * Math::PI / 180.0);
			RangeY[i] = Range[i] * Math::Cos(i * Resolution * Math::PI / 180.0);
			//store laser data 
			LaserPtr->XRange[i] = RangeX[i];
			LaserPtr->YRange[i] = RangeY[i];	
			if (i < 5) 
			{
				Console::WriteLine("{0,10:F3} {1,10:F3} ", RangeX[i], RangeY[i]);
			}

			if (PMSMPtr->Shutdown.flags.Laser == 1)
			{
				exit(0);
			}

		}	
			

		if (PMSMPtr->Shutdown.flags.Laser == 1) break;
		if (_kbhit()) break;
		System::Threading::Thread::Sleep(200);

	}

	Stream->Close();
	Client->Close();

	//Console::ReadKey();
	Console::WriteLine("Laser terminated normally.");
	//Console::ReadKey();
	return 0;
}