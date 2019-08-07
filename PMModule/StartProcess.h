#pragma once

//REMINDER: When you change the number of modules, please change the NUM_UNITS 
#define	NUM_UNITS 5 //the number of modules-1
bool IsProcessRunning(const char* processName);
//The following file gives some useful code fragments 
//for process management. You should fill out the main function with the following steps:
//1) start all processes
//2) while the PM shutdown flag is not set, keep looping through the heartbeat process

//defining start up sequence

// Start up sequence
TCHAR* Units[NUM_UNITS] = //
{
	TEXT("LaserModule.exe"),
	TEXT("GPSModule.exe"),
	TEXT("XboxModule.exe"),
	TEXT("VehicleModule.exe"),
	TEXT("DisplayModule.exe"),

};

// Module execution based variable declarations
STARTUPINFO s[NUM_UNITS];
PROCESS_INFORMATION p[NUM_UNITS];

// Starting the processes
int StartProcess() {
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
				_getch();
				return -1;
			}
		}
		std::cout << "Started: " << Units[i] << std::endl;
		Sleep(1000);
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

void AllShutDown(PM* PMSMPtr) {
	PMSMPtr->Shutdown.flags.Laser = 1;
	System::Threading::Thread::Sleep(200);
	PMSMPtr->Shutdown.flags.GPS = 1;
	System::Threading::Thread::Sleep(200);
	PMSMPtr->Shutdown.flags.Xbox = 1;
	System::Threading::Thread::Sleep(200);
}