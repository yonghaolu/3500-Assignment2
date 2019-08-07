//#include "LaserStruct.h"
////#include<SMStruct.h>
//
//using namespace System;
//using namespace System::Net::Sockets;
//using namespace System::Net;
//using namespace System::Text;
//
//LiDAR::LiDAR(String^ ipaddress, int port) {
//	IPAddress = ipaddress;
//	PortNumber = port;
//	Client = gcnew TcpClient(IPAddress, PortNumber);
//	AskScan = gcnew String("sRN LMDscandata");
//	SendData = gcnew array<unsigned char>(16);
//	ReadData = gcnew array<unsigned char>(2500);
//	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
//};
//
//
//bool LiDAR::Connect() {
//	Client->Connect(IPAddress, PortNumber);
//
//	if (Client->Connected)
//	{
//		Console::WriteLine("Connected to LIDAR");
//	}
//	else
//	{
//		Console::WriteLine("Cannot find LIDAR");
//		return 0;
//	}
//
//	// Configure connection
//	Client->NoDelay = true;
//	Client->ReceiveTimeout = 2000;//ms
//	Client->SendTimeout = 500;//ms
//	Client->ReceiveBufferSize = 2048;
//	Client->SendBufferSize = 1024;
//}
//
//void LiDAR::GetXYRangeData(array<String^>^ Fragments, Laser* Ptr) {
//	for (int i = 0; i < 6; i++)
//	{
//		Range[i] = System::Convert::ToInt32(Fragments[26 + i], 16);
//		RangeX[i] = Range[i] * Math::Sin(i * Resolution * Math::PI / 180.0);
//		RangeY[i] = -Range[i] * Math::Cos(i * Resolution * Math::PI / 180.0);
//		Ptr->XRange[i] = RangeX[i];
//		Ptr->YRange[i] = RangeY[i];
//		Console::WriteLine("{0,10:F3} {1,10:F3} ", RangeX[i], RangeY[i]);
//	}
//}
//
//void LiDAR::RequestScan()
//{
//	Stream->WriteByte(0x02);
//	Stream->Write(SendData, 0, SendData->Length);
//	Stream->WriteByte(0x03);
//
//	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
//	System::Threading::Thread::Sleep(10);
//}
//
//void LiDAR::ScanData(Laser* Ptr)
//{
//	// Read the incoming data
//	Stream->Read(ReadData, 0, ReadData->Length);
//
//	// Convert incoming data from an array of unsigned char bytes to an ASCII string
//	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
//	//Console::WriteLine(ResponseData);
//
//	Fragments = ResponseData->Split(' ');
//
//	StartAngle = System::Convert::ToInt32(Fragments[23], 16);
//	Resolution = System::Convert::ToInt32(Fragments[24], 16) / 10000.0;
//	NumRanges = System::Convert::ToInt32(Fragments[25], 16);
//	Ptr->Resolution = Resolution;
//	Ptr->NumPoints = NumRanges;
//	Ptr->startAngle = StartAngle;
//
//	std::cout << "Start Angle: " << StartAngle << " Resolution: " << Resolution << " Number of Ranges: " << NumRanges << std::endl;
//
//	Range = gcnew array<double>(NumRanges);
//	RangeX = gcnew array<double>(NumRanges);
//	RangeY = gcnew array<double>(NumRanges);
//}
//
//void LiDAR::IDAutentication()
//{
//	ID = gcnew String("5125710\n");
//	SendID = System::Text::Encoding::ASCII->GetBytes(ID);
//	NetworkStream^ Stream = Client->GetStream();
//
//	//send ID to LiDAR
//	Stream->Write(SendID, 0, SendID->Length);
//	System::Threading::Thread::Sleep(10);
//
//}
//
//void LiDAR::CloseAll()
//{
//	Stream->Close();
//	Client->Close();
//
//}
