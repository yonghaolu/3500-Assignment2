#using <System.dll>

#include <SMObject.h>
#include <SMStruct.h> 
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>
#include <Windows.h>
#include <string.h>
#include <stdio.h>

using namespace System;
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;

int main() {
	SMObject PMObj(_TEXT("ProcessManagemnet"), sizeof(PM));
	SMObject UGVObj(_TEXT("Vehicle"), sizeof(UGV));
	SMObject XBoxObj(_TEXT("XBoxControl"), sizeof(Remote));
	PM* PMSMPtr = nullptr;
	UGV* UGVPtr = nullptr;
	Remote* XBoxSMPtr = nullptr;
	PMObj.SMAccess();
	if (PMObj.SMAccessError)
	{
		Console::WriteLine("Shared memory access failed");
		return -2;
	}
	UGVObj.SMAccess();
	if (UGVObj.SMAccessError)
	{
		Console::WriteLine("Shared memory access failed");
		return -2;
	}
	XBoxObj.SMAccess();

	PMSMPtr = (PM*)PMObj.pData;
	UGVPtr = (UGV*)UGVObj.pData;
	XBoxSMPtr = (Remote*)XBoxObj.pData;
	
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;

	array<unsigned char>^ SendAuthor;
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	
	// Creat TcpClient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", MotorControlPort); //port number:25000
	if (Client->Connected)
	{
		Console::WriteLine("Connected to UGV");
	}
	else
	{
		Console::WriteLine("Cannot find UGV");
		//return 0;
	}

	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	NetworkStream^ Stream = Client->GetStream();
	
	//send ID to UGV
	String^ Author = gcnew String("5125710\n");
	SendAuthor = System::Text::Encoding::ASCII->GetBytes(Author);
	Stream->Write(SendAuthor, 0, SendAuthor->Length);

	PMSMPtr->TimeStamp[Vehicle_time] =0;
	int i = 0;
	int Flag = 1;
	double speed = 0;
	double steering = 0;
	while (!PMSMPtr->Shutdown.flags.Vehicle) {
		PMSMPtr->TimeStamp[Vehicle_time] = GetTimeStamp(PMSMPtr);

		if ((PMSMPtr->TimeStamp[Vehicle_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime) {
			i++;
			if ((i > 20) && ((PMSMPtr->TimeStamp[Vehicle_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime)) {
				PMSMPtr->Shutdown.Status = 0xff;
				std::cout << "PM error! Shut down all." << std::endl;
			}
			std::cout << "PM is alive." << std::endl;
		}

		speed = XBoxSMPtr->SetSpeed;
		steering = XBoxSMPtr->SetStreeing;
	
		Flag = !Flag;
		char send[50];
		sprintf(send,"#%f %f %d#", steering, speed, Flag);
	
		String^ SendString = gcnew String(send);
		SendData = System::Text::Encoding::ASCII->GetBytes(SendString);

		Stream->Write(SendData, 0, SendData->Length);

		std::cout << "Set Speed: " << XBoxSMPtr->SetSpeed << " Set streeing: " << XBoxSMPtr->SetStreeing*40 << std::endl;
		std::cout << "Flag: " << Flag << std::endl;
		if (_kbhit()) break;
		System::Threading::Thread::Sleep(200);
	}
	Stream->Close();
	Client->Close();

	Console::WriteLine("UGV terminated normally.");

	return 0;
}
