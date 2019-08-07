#include <SMObject.h>
#include "SMStruct.h" 
#include <conio.h>
#include <iostream>
#include <Windows.h>
#include <string.h>
#include "Start_Close.h"

#define Critical 1

using namespace System;
using namespace System::Threading;

int main() {
	SetConsoleTitle("Process Management.exe");// rename console title

	//Build Shared Memory objects
	SMObject PMObj(_TEXT("ProcessManagemnet"), sizeof(PM));
	SMObject LaserObj(_TEXT("LaserData"), sizeof(Laser));
	SMObject GPSObj(_TEXT("GPS"), sizeof(GPS));
	SMObject XBoxObj(_TEXT("XBoxControl"), sizeof(Remote));
	SMObject UGVObj(_TEXT("Vehicle"), sizeof(UGV));
	PMObj.SMCreate();
	LaserObj.SMCreate();
	GPSObj.SMCreate();
	XBoxObj.SMCreate();
	UGVObj.SMCreate();

	PM* PMSMPtr = nullptr;

	if (PMObj.SMCreateError)
	{
		Console::WriteLine("Shared memory creation failed");
		return -1;
	}

	PMObj.SMAccess();
	if (PMObj.SMAccessError)
	{
		Console::WriteLine("Shared memory access failed");
		return -2;
	}
	PMSMPtr = (PM*)PMObj.pData;

	//start Modules one by one
	Console::WriteLine("Ready to start!");
	StartTime(PMSMPtr); //initial start time 
	PMSMPtr->TimeStamp[PM_time] = GetTimeStamp(PMSMPtr);
	StartAllProcess();

	//Prepaer to run Process management Module
	PMSMPtr->Shutdown.Status = 0x00;
	
	for (int a = 0; a < 6; a++) {
		PMSMPtr->TimeStamp[a] = 0;
	}

	StartTime(PMSMPtr); //initial start time 
	while (!PMSMPtr->Shutdown.flags.PM) {
		PMSMPtr->TimeStamp[PM_time] = GetTimeStamp(PMSMPtr);
		std::cout << "PM      time difference: " << PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[PM_time] << std::endl;
		std::cout << "Laser   time difference: " << PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[Laser_time] << std::endl;
		std::cout << "GPS     time difference: " << PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[GPS_time] << std::endl;
		std::cout << "XBox    time difference: " << PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[XBox_time] << std::endl;
		std::cout << "Display time difference: " << PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[Display_time] << std::endl;
		std::cout << "Vehicle time difference: " << PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[Vehicle_time] << std::endl;
		std::cout << " " << std::endl;
			
		for (int i = 0; i < 6 ; i++) {
			if (PMSMPtr->TimeStamp[PM_time] - PMSMPtr->TimeStamp[i] > CheckPMTime) {
				if (Process[i] == Critical) {
					std::cout <<"Critical "<< ProcessName[i] << " running overtime, shut down all process." << std::endl;
					PMSMPtr->Shutdown.Status = 0xFF; break;
					Console::ReadKey();
				}
				else {
					if (!IsProcessRunning(ProcessName[i])) {
						Restart(i); //restart
					}
					else {
						killProcessByName(ProcessName[i]); //kill
						Restart(i); //restart
					}
				}

			}
		}
		if (_kbhit()) break;
		System::Threading::Thread::Sleep(200); //2 seconds

	}

	//Close ALL Modules
	ShutDownAll(PMSMPtr);
	Console::WriteLine("All process are shut down");

	Console::WriteLine("Process management terminated normally.");

	return 0;
}
