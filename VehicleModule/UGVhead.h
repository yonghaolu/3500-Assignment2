#pragma once
#using <System.dll>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "SMObject.h"
#include "SMStruct.h"
#include <conio.h>
#include <string>

using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Text;

ref class Weeder
{
private:
	double speed;
	double streer;
	bool flag;
	int PortNumber;

	array<unsigned char>^ buffer;
	array<unsigned char>^ SendAuthor;
	NetworkStream^ Stream;
	TcpClient^ Client;
	String^ IPAddress;
	String^ ID;
	
public:
	Weeder(String^ ipaddress, int port);
	void IDAutentication();
	void SendData(double steer, double speed);
	bool Connect();
	void CloseAll();
};


Weeder::Weeder(String^ ipaddress, int port)
{
	IPAddress = ipaddress;
	PortNumber = port;
	Client = gcnew TcpClient(IPAddress, PortNumber);

}

bool Weeder::Connect()
{
	Client->Connect(IPAddress, PortNumber);

	if (Client->Connected)
	{
		Console::WriteLine("Connected to LIDAR");
	}
	else
	{
		Console::WriteLine("Cannot find LIDAR");
		return 0;
	}

	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 2000;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 2048;
	Client->SendBufferSize = 1024;
}

void Weeder::IDAutentication() 
{
	String^ Author = gcnew String("5125710\n");
	SendAuthor = System::Text::Encoding::ASCII->GetBytes(Author);
	NetworkStream^ Stream = Client->GetStream();
	//send ID to UGV
	Stream->Write(SendAuthor, 0, SendAuthor->Length);
	System::Threading::Thread::Sleep(10);
	//Stream->Read(ReadData, 0, ReadData->Length);
}

void Weeder::CloseAll()
{
	Stream->Close();
	Client->Close();

}

void Weeder::SendData(double steer,double speed) 
{
	char car[32];
	sprintf(car, "#%f %f %d#", steer * 40, speed, flag);


	flag = !flag;
}