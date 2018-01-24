#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

using namespace std;
//////////////////////////////////////////////////
// BHOP HACK BY NULL TERMINATOR (YT TUTORIAL)  ///
//////////////////////////////////////////////////

//Handle for desired process
HANDLE hProc;

//Structure that stores information about the current process in the snapshot loop
PROCESSENTRY32 ProcEntry;

//Global DWORD to store process ID so we can call RPM, WPM, etc.
DWORD pID;


//Handle for the desired module we want
HANDLE hMod;

//Structure that stores information about the current module in the snapshot loop
MODULEENTRY32 mEntry;


//Attach to process (Gain a handle to it and open access to it)
void getProc(char* procName)
{
	//Declaring structure size
	ProcEntry.dwSize = sizeof(ProcEntry);

	//Creating snapshot of processes so we can loop through it
	hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	//While there is a next process in the snapshot keep looping
	while (Process32Next(hProc, &ProcEntry))
	{
		//Seeing if the current process in the snapshot is the one we want by comparing
		//it's exe filename to the one we are looking for ("csgo.exe" in our case)
		if (!strcmp((char*)ProcEntry.szExeFile, procName))
		{
			//If it is the process we want to attach to, store the proc ID in the global var
			pID = ProcEntry.th32ProcessID;
			CloseHandle(hProc);

			//Granting access to this proc
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
			return;
		}

	}

	//Error handling because we're excellent programmers.
	cout << "Process wasnt found in snapshot lmao trash.\n";
}

DWORD getMod(LPSTR ModuleName)
{
	//Creating snapshot of modules in the process
	hMod = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	
	mEntry.dwSize = sizeof(mEntry);

	while (Module32Next(hMod, &mEntry))
	{
		//Comparing names of current module in loop and the name of the 
		//module we want.
		if (!strcmp((char*)mEntry.szModule, ModuleName))
		{
			CloseHandle(hMod);
			return (DWORD)mEntry.modBaseAddr;
		}

	}

	//Error handling because we're excellent programmers.
	cout << "Module wasnt found lol\n";

}

//WPM function wrappper
template <class cData>
void Write(DWORD dwAddress, cData value)
{
	WriteProcessMemory(hProc, (LPVOID)dwAddress, &value, sizeof(cData), NULL);
}

//RPM function wrappper
template <class cData>
cData Read(DWORD dwAddress)
{
	cData readValue;
	ReadProcessMemory(hProc, (LPVOID)dwAddress, &readValue, sizeof(cData), NULL);
	return readValue;
}

/* PLAYER FLAG VALUES */
/*
FL_ON_GROUND                     257
FL_ON_GROUND_CROUCHED             263
FL_IN_AIR_STAND                    256
FL_IN_AIR_MOVING_TO_STAND         258
FL_ON_GROUND_MOVING_TO_STAND     259
FL_IN_AIR_MOVING_TO_CROUCH         260
FL_ON_GROUND_MOVING_TO_CROUCH     261
FL_IN_AIR_CROUCHED                 262
FL_IN_WATER                     1280
FL_IN_PUDDLE                     1281
FL_IN_WATER_CROUCHED             1286
FL_IN_PUDDLE_CROUCHED             1287
*/

int main()
{
	//Attach to csgo so we can WPM and RPM
	getProc("csgo.exe");

	//Get base address of client.dll so we can offset from it
	DWORD clientDllBase = getMod("client.dll");

	cout << "Bunnyhop hack by Null Terminator on YT\n Follow me @crying_softly :D\n";

	//Main cheat loop
	while (true)
	{
		//Get local player base
		DWORD lclPlr = Read<DWORD>(clientDllBase + 0xA9D0DC);

		//Get current player flags (Stance value)
		int localPlayerStance = Read<int>(lclPlr + 0x100);
		
		//If holding space and the local player is on the ground
		if (GetAsyncKeyState(VK_SPACE) && localPlayerStance == 257)
		{
			//Force jump
			Write<int>(clientDllBase + 0x4F11134, 1);
			//Wait till off ground
			Sleep(30);
			//Stop forcing jump
			Write<int>(clientDllBase + 0x4F11134, 0);
		}
	}
	

}
