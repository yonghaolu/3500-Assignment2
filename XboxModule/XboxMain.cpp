#include <SMObject.h>
#include <SMStruct.h> 
#include <conio.h>
#include <tlhelp32.h>
#include <iostream>
#include <Windows.h>
#include <string.h>
#include <xinput.h>
#include "XBoxController.h"
#include "XInputWrapper.h"

#include <cmath>


using namespace std;
using namespace System;
using namespace System::Threading;

#define max_value 32767.0


int main() {
	bool Quit = false;
	XINPUT_BATTERY_INFORMATION BatteryInformation;
	XINPUT_STATE State;
	XINPUT_VIBRATION Vibration;
	XInputEnable(true);
	XInputGetBatteryInformation(0, BATTERY_DEVTYPE_GAMEPAD, &BatteryInformation);
	switch (BatteryInformation.BatteryLevel)
	{
	case BATTERY_LEVEL_EMPTY: cout << "Battery level empty " << endl;
		break;
	case BATTERY_LEVEL_LOW: cout << "Battery level low " << endl;
		break;
	case BATTERY_LEVEL_MEDIUM: cout << "Battery level medium " << endl;
		break;
	case BATTERY_LEVEL_FULL: cout << "Battery level full" << endl;
	}

	int DeadZoneRange = 2000;  // deadzone
	short LX;
	short LY;
	short RX;
	short RY;
	float RadiusL;
	float RadiusR;
	float normalizedLY;  //speed
	float normalizedRX;	 //steering
	float normalizedRadiusL = 0;
	float normalizedRadiusR = 0;

	SMObject PMObj(_TEXT("ProcessManagemnet"), sizeof(PM));
	SMObject XBoxObj(_TEXT("XBoxControl"), sizeof(Remote));
	PM* PMSMPtr = nullptr;
	Remote* XBoxSMPtr = nullptr;
	PMObj.SMAccess();
	XBoxObj.SMAccess();
	PMSMPtr = (PM*)PMObj.pData;
	XBoxSMPtr = (Remote*)XBoxObj.pData;
	
	PMSMPtr->TimeStamp[XBox_time] = 0;// PMSMPtr->TimeStamp[PM_time];
	int i = 0;
	while (!PMSMPtr->Shutdown.flags.Laser) {
		PMSMPtr->TimeStamp[XBox_time] = GetTimeStamp(PMSMPtr);
		//Console::WriteLine("XBox Timestamp: {0,10:F3}", PMSMPtr->TimeStamp[XBox_time]);
		//std::cout << "i: " << i << std::endl;
		if((PMSMPtr->TimeStamp[XBox_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime) {
			i++;
			if ((i > 20) && ((PMSMPtr->TimeStamp[XBox_time] - PMSMPtr->TimeStamp[PM_time]) > CheckPMTime)) {
				PMSMPtr->Shutdown.Status = 0xff;
				std::cout << "PM error! Shut down all." << std::endl;
			}

		}
		std::cout << "PM is alive." << std::endl;

		if (Quit == 0)
		{
			if (XInputGetState(0, &State) == ERROR_DEVICE_NOT_CONNECTED)
			{
				cout << "Joystick not connected " << endl;
				XBoxSMPtr->SetSpeed = 0.0;
				XBoxSMPtr->SetStreeing = 0.0;
				return 1;
			}
			
			if (State.Gamepad.wButtons == 1){// up button
				Quit = true;
				PMSMPtr->Shutdown.Status = 0xFF; //shut down all
			}

			LX = 0;
			LY = State.Gamepad.sThumbLY;
			RX = State.Gamepad.sThumbRX;
			RY = 0;

			RadiusL = sqrt(LX * LX + LY * LY);
			RadiusR = sqrt(RX * RX + RY * RY);
			
			//left Joystick control speed
			if (RadiusL > DeadZoneRange)
			{
				normalizedLY = LY / RadiusL;
				if (RadiusL > 32767)
				{
					RadiusL = 32767;
				}
				RadiusL -= DeadZoneRange;
				normalizedRadiusL = RadiusL / (32767 - DeadZoneRange);
				
				XBoxSMPtr->SetSpeed = normalizedLY * normalizedRadiusL * 1.0;
			}
			else
			{
				RadiusL = 0;
				normalizedRadiusL = 0;
				XBoxSMPtr->SetSpeed = 0;
			}
			
			//right Joystick control streeing
			if (RadiusR > DeadZoneRange)
			{
				normalizedRX = RX / RadiusR;	
				if (RadiusR > 32767)
				{
					RadiusR = 32767;
				}
				RadiusR -= DeadZoneRange;
				normalizedRadiusR = RadiusR / (32767 - DeadZoneRange);

				XBoxSMPtr->SetStreeing = normalizedRX * normalizedRadiusR * 40.0;
			}
			else
			{
				RadiusR = 0;
				normalizedRadiusR = 0;
				XBoxSMPtr->SetStreeing = 0;
			}

			//std::cout << "TimeStamp: " << PMSMPtr->TimeStamp[XBox_time] << "  State.Gamepad.wButtons: " << State.Gamepad.wButtons << std::endl;
			//std::cout << "Gamepad.sThumbLY: " << State.Gamepad.sThumbLY << " Gamepad.sThumbRX: " << State.Gamepad.sThumbRX << std::endl;
			std::cout << "Set Speed: " << XBoxSMPtr->SetSpeed << " Set streeing: " << XBoxSMPtr->SetStreeing << std::endl;
			std::cout << "" << std::endl;
		}

		if (PMSMPtr->Shutdown.flags.Laser == 1) break;
		System::Threading::Thread::Sleep(WAIT_TIME);
	}

	//Console::ReadKey();
	Console::WriteLine("Process management terminated normally.");
	//Console::ReadKey();
	return 0;
}



