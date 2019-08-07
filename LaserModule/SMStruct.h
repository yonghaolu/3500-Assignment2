#pragma once
#include <iostream>
#define LaserPort        23000
#define GPSPort          24000
#define	MotorControlPort 25000

#define CheckPMTime 2100
#define	CheckTime  (WAIT_TIME + 10)
#define WAIT_TIME  50
#define PM_time    0
#define GPS_time   1
#define Laser_time 2
#define XBox_time  3
#define Vehicle_time 4
#define Display_time 5
int Process[6] = {1,  0,    1  ,  1,    0,      0}; //1->Critical; 0->Non-Critical
				//PM, GPS, Laser, Xbox, Vehicle, Display
				// 0   1     2       3    4       5
TCHAR* ProcessName[6] = //
{
	TEXT("PM.exe"),
	TEXT("GPSModule.exe"),   // 0
	TEXT("LaserModule.exe"),   //1
	TEXT("XboxModule.exe"),   //2
	TEXT("VehicleModule.exe"),  //3
	TEXT("DisplayModule.exe"),

};

struct Moduleflags
{
	unsigned char PM      : 1,
				  GPS     : 1,
				  Laser   : 1,
				  Xbox    : 1,
				  Vehicle : 1,
				  Display : 1,
				  Unused  : 2;
};

union ExecFlags
{
	unsigned char Status;
	Moduleflags flags;
};

struct Remote
{
	double SetSpeed;
	double SetStreeing;
	double XBoxTimeStamp;
};

struct PM
{
	ExecFlags Shutdown;
	LARGE_INTEGER Li;
	double PCFreq;
	__int64 CounterStart;
	double TimeStamp[8];//0:PM; 1:Laser; 
};						//2:GPS; 3:Xbox;
						//4:Vehicle; 5:Display
struct GPS
{
	double Easting;
	double Northing;
	double Height;
	double GPSTimeStamp;
};

struct Laser
{
	int NumPoints;
	double Resolution;
	double startAngle;
	double XRange[600];
	double YRange[600];
	double LaserTimeStamp;

};

struct Vehicle
{
	double ActualSpeed;
	double ActualStreeing;
	double UGVTimeStamp;
};

void StartTime(PM* PMSMPtr)
{
	if (!QueryPerformanceFrequency(&PMSMPtr->Li))
		std::cout << "QueryPerformanceFrequency failed!" << std::endl;

	PMSMPtr->PCFreq = double(PMSMPtr->Li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&PMSMPtr->Li);
	PMSMPtr->CounterStart = PMSMPtr->Li.QuadPart;
}

double GetTimeStamp(PM* PMSMPtr)
{
	QueryPerformanceCounter(&PMSMPtr->Li);
	return double(PMSMPtr->Li.QuadPart - PMSMPtr->CounterStart) / PMSMPtr->PCFreq;
}