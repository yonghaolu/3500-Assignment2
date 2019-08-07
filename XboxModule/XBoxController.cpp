#include <iostream>
#include <Windows.h>
#include <xinput.h>
#include "XBoxController.h"
#include "XInputWrapper.h"
#include <cmath>


GamePad::XBoxController::XBoxController(XInputWrapper* xinput, DWORD id)
{
	XInputEnable(TRUE);
	this->xinput = xinput;
	this->ID = id;
}

DWORD GamePad::XBoxController::GetControllerId()
{
	return ID;
}

bool GamePad::XBoxController::IsConnected()
{
	xinput->XInputGetState(ID, &state);
	xinput->XInputGetBatteryInformation(ID, BATTERY_DEVTYPE_GAMEPAD, &BatteryInformation);
	if (BatteryInformation.BatteryType != BATTERY_TYPE_DISCONNECTED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//right A,B,X,Y
bool GamePad::XBoxController::PressedA()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedB()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedX()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedY()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0) {
		return true;
	}
	else
	{
		return false;
	}
}
//sholder
bool GamePad::XBoxController::PressedLeftShoulder()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedRightShoulder()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//Dpad
bool GamePad::XBoxController::PressedRightDpad()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedLeftDpad()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedUpDpad()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedDownDpad()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//start
bool GamePad::XBoxController::PressedStart()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//back
bool GamePad::XBoxController::PressedBack()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//Thumb
bool GamePad::XBoxController::PressedLeftThumb()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool GamePad::XBoxController::PressedRightThumb()
{
	xinput->XInputGetState(ID, &state);
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//Trigger
BYTE GamePad::XBoxController::LeftTriggerLocation()
{
	xinput->XInputGetState(ID, &state);
	return state.Gamepad.bLeftTrigger;
}
BYTE GamePad::XBoxController::RightTriggerLocation()
{
	xinput->XInputGetState(ID, &state);
	return state.Gamepad.bRightTrigger;
}

GamePad::Coordinate GamePad::XBoxController::LeftThumbLocation()
{
	xinput->XInputGetState(ID, &state);
	GamePad::Coordinate LTC(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
	short LX = LTC.GetX();
	short LY = LTC.GetY();
	float Radius = sqrt(LX * LX + LY * LY);
	float normalizedRadius = 0;

	if (Radius > DeadZoneRange)
	{
		float normalizedLX = LX / Radius;
		float normalizedLY = LY / Radius;
		if (Radius > 32767)
		{
			Radius = 32767;
		}
		Radius -= DeadZoneRange;
		normalizedRadius = Radius / (32767 - DeadZoneRange);
		LTC.SetX(normalizedLX * normalizedRadius * 32767);
		LTC.SetY(normalizedLY * normalizedRadius * 32767);
	}
	else
	{
		Radius = 0;
		normalizedRadius = 0;
		LTC.SetX(0);
		LTC.SetY(0);
	}
	return LTC;
}
GamePad::Coordinate GamePad::XBoxController::RightThumbLocation()
{
	xinput->XInputGetState(ID, &state);
	GamePad::Coordinate RTC(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
	short RX = RTC.GetX();
	short RY = RTC.GetY();
	float Radius = sqrt(RX * RX + RY * RY);
	float normalizedRadius = 0;

	if (Radius > DeadZoneRange)
	{
		float normalizedRX = RX / Radius;
		float normalizedRY = RY / Radius;
		if (Radius > 32767)
		{
			Radius = 32767;
		}
		Radius -= DeadZoneRange;
		normalizedRadius = Radius / (32767 - DeadZoneRange);
		RTC.SetX(normalizedRX * normalizedRadius * 32767);
		RTC.SetY(normalizedRY * normalizedRadius * 32767);
	}
	else
	{
		Radius = 0;
		normalizedRadius = 0;
		RTC.SetX(0);
		RTC.SetY(0);
	}

	return RTC;
}

//Vibrate
void GamePad::XBoxController::Vibrate(WORD left, WORD right)
{
	xinput->XInputGetState(ID, &state);
	Vibration.wLeftMotorSpeed = left; // 0-65535
	Vibration.wRightMotorSpeed = right; //0-65535
	xinput->XInputSetState(ID, &Vibration);

}

void GamePad::XBoxController::SetDeadzone(unsigned int radius)
{
	if (radius < 32767)
	{
		DeadZoneRange = radius;
	}
	else if (radius > 32767)
	{
		std::cout << "Please enter number between 0 to 32767" << std::endl;
	}
}

