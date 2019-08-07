#pragma once
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
//REMINDER: When you change the number of modules, please change the NUM_UNITS 
#define	NUM_UNITS 5 //the number of modules-1
#define Close_Delay 300
bool IsProcessRunning(const char* processName);
//The following file gives some useful code fragments 
//for process management. You should fill out the main function with the following steps:
//1) start all processes
//2) while the PM shutdown flag is not set, keep looping through the heartbeat process

//defining start up sequence

// Start up sequence
TCHAR* Units[NUM_UNITS] = //
{
	TEXT("GPSModule.exe"),   // 0
	TEXT("LaserModule.exe"),   //1
	TEXT("XboxModule.exe"),   //2
	TEXT("VehicleModule.exe"),  //3
	TEXT("DisplayModule.exe"), //4

};

// Module execution based variable declarations
STARTUPINFO s[NUM_UNITS];
PROCESS_INFORMATION p[NUM_UNITS];

// Starting the processes
void StartAllProcess() {
	for (int i = 0; i < NUM_UNITS; i++)
	{
		// Check if each process is running
		if (!IsProcessRunning(Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			// Start the child processes.

			if (!CreateProcess(NULL,   // No module name (use command line)
				Units[i],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_NEW_CONSOLE,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory
				&s[i],            // Pointer to STARTUPINFO structure
				&p[i])           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				//_getch();
				//return -1;
			}
		}
		std::cout << "Started: " << Units[i] << std::endl;
		//Sleep(100);
	}
}

//Is provess running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}

void ShutDownAll(PM* PMSMPtr) {
	PMSMPtr->Shutdown.flags.Vehicle = 1;
	System::Threading::Thread::Sleep(Close_Delay);
	PMSMPtr->Shutdown.flags.Laser = 1;
	System::Threading::Thread::Sleep(Close_Delay);
	PMSMPtr->Shutdown.flags.GPS = 1;
	System::Threading::Thread::Sleep(Close_Delay);
	PMSMPtr->Shutdown.flags.Xbox = 1;
	System::Threading::Thread::Sleep(Close_Delay);
	PMSMPtr->Shutdown.flags.Display = 1;
	System::Threading::Thread::Sleep(Close_Delay);
}

//Reference: h ttps://stackoverflow.com/questions/7956519/how-to-kill-processes-by-name-win32-api 
void killProcessByName(const char* filename) //Input: ProcessName[i]
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

void Restart(int i) { //input: the No. of process
	ZeroMemory(&s[i], sizeof(s[i]));
	s[i].cb = sizeof(s[i]);
	ZeroMemory(&p[i], sizeof(p[i]));
	// Start the child processes.

	if (!CreateProcess(NULL,   // No module name (use command line)
		ProcessName[i],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&s[i],            // Pointer to STARTUPINFO structure
		&p[i])           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("%s failed (%d).\n", ProcessName[i], GetLastError());
		_getch();
		//return -1;
	}
	std::cout << "Restarted: " <<ProcessName[i] << std::endl;
	Sleep(500);
}