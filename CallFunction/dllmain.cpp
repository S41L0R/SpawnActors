// dllmain.cpp : Defines the entry point for the DLL application.
#include <sstream>
#include <map>
#include <iostream>

#include "util/BotwEdit.h"
#include "UI.h"

#include "Windows.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"



// This stuff here was yoinked from BetterVR
// -----------------------------------------
union FPR_t {
	double fpr;
	struct
	{
		double fp0;
		double fp1;
	};
	struct
	{
		uint64_t guint;
	};
	struct
	{
		uint64_t fp0int;
		uint64_t fp1int;
	};
};

struct PPCInterpreter_t {
	uint32_t instructionPointer;
	uint32_t gpr[32];
	FPR_t fpr[32];
	uint32_t fpscr;
	uint8_t crNew[32]; // 0 -> bit not set, 1 -> bit set (upper 7 bits of each byte must always be zero) (cr0 starts at index 0, cr1 at index 4 ..)
	uint8_t xer_ca;  // carry from xer
	uint8_t LSQE;
	uint8_t PSE;
	// thread remaining cycles
	int32_t remainingCycles; // if this value goes below zero, the next thread is scheduled
	int32_t skippedCycles; // if remainingCycles is set to zero to immediately end thread execution, this value holds the number of skipped cycles
	struct
	{
		uint32_t LR;
		uint32_t CTR;
		uint32_t XER;
		uint32_t UPIR;
		uint32_t UGQR[8];
	}sprNew;
};
// -----------------------------------------

typedef void (*osLib_registerHLEFunctionType)(const char* libraryName, const char* functionName, void(*osFunction)(PPCInterpreter_t* hCPU));


#pragma pack(1)
struct Data { // This is reversed compared to the gfx pack because we read as big endian.
	char name[64]; // (Not long enough for all actor names...)
	uint8_t actorStorage[76];
	uint8_t handle[28];
	uint8_t storage[88]; // This is just the remainder of storage that isn't repurposed and named

	int f_r10;
	int f_r9;
	int f_r8;
	int f_r7;
	int f_r6;
	int f_r5;
	int f_r4;
	int f_r3;

	int n_r10; 
	int n_r9; 
	int n_r8;
	int n_r7;
	int n_r6;
	int n_r5;
	int n_r4;
	int n_r3;

	int o_lr;
	int o_r10;
	int o_r9;
	int o_r8;
	int o_r7;
	int o_r6;
	int o_r5;
	int o_r4;
	int o_r3;
	
	int fnAddr;
	int interceptRegisters;
	int enabled;
};

// ---------------------------------------------------------------------------------
// This is an example function call.
// - Feel free to expand / change -
// Note: This does not take into account stuff like actually setting desired params.
// ---------------------------------------------------------------------------------
std::map<char, std::string> keyCodeMap;

std::map<char, bool> prevKeyStateMap; // Used for key press logic - keeps track of previous key state


MemoryInstance* memInstance;

bool setup = false;

void mainFn(PPCInterpreter_t* hCPU) {
	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to - REQUIRED

	if (!setup) {
		uint32_t linkPosOffset;
		memInstance->memory_readMemoryBE(0x11344418, &linkPosOffset); // Some random reference to link's position that seems to work...
		memInstance->linkData.PosX = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x50); // oh wait,
		memInstance->linkData.PosY = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x54); // it's 
		memInstance->linkData.PosZ = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x58); // inconsistent

		if (*memInstance->linkData.PosX == 0.f && *memInstance->linkData.PosY == 0.f && *memInstance->linkData.PosZ == 0.f)
			Console::LogPrint("Try again! 0 0 0 Glitch.");

		uint32_t startData = hCPU->gpr[3];
		Data data;
		memInstance->memory_readMemoryBE(startData, &data); // Just make sure to intercept stuff..
		data.interceptRegisters = true;
		memInstance->memory_writeMemoryBE(startData, data);

		setup = true;
		return;
	}

	uint32_t startData = hCPU->gpr[3];
	Data data;
	memInstance->memory_readMemoryBE(startData, &data); 

	data.interceptRegisters = true; // Just make sure to intercept stuff..

	int hashId = 0;
	memInstance->memory_readMemoryBE(data.f_r6 + (6 * 4), &hashId);
	int thing;
	if (hashId == -833019966) {
		int thing = 5; // Debug code, but maybe at one point have it only grab the hashId of a specific actor to resolve inconsistencies..
	}


	// Basic key press logic to make sure holding down doesn't spam triggers
	for (std::map<char, std::string>::iterator keyCodeMapIter = keyCodeMap.begin(); keyCodeMapIter != keyCodeMap.end(); ++keyCodeMapIter) {
		char key = keyCodeMapIter->first;

		bool keyPressed = false;
		if (GetKeyState(key) & 0x8000)
			keyPressed = true;
	    

		if (keyPressed && !prevKeyStateMap.find(keyCodeMapIter->first)->second) { // Make sure the key is pressed this frame and wasn't last frame

			uint32_t startData = hCPU->gpr[3]; // Find where data starts from r3


			// Lets set any data that our params will reference:
			// -------------------------------------------------

			// Copy needed data over to our own storage to not override actor stuff
			memInstance->memory_readMemoryBE(data.f_r6, &data.actorStorage);
			int actorStorageLocation = startData + sizeof(data) - sizeof(data.name) - sizeof(data.actorStorage);
			memInstance->memory_readMemoryBE(data.f_r7, &data.handle); // Not sure if this cooresponds to the handle, but oh well
			int handleLocation = startData + sizeof(data) - sizeof(data.name) - sizeof(data.actorStorage) - sizeof(data.handle);

			// Set actor pos to link pos
			float posX = (float)*memInstance->linkData.PosX;
			float posY = (float)*memInstance->linkData.PosY;
			float posZ = (float)*memInstance->linkData.PosZ;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (8 * 4) - 0], &posX, sizeof(float)); // I have less idea
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (8 * 4) - 4], &posY, sizeof(float)); // what's going on with that address
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (8 * 4) - 8], &posZ, sizeof(float)); // than you do... wait I'm lying

			// We want to make sure there's a fairly high traverseDist
			float traverseDist = 1000.f;
			short traverseDistInt = (short)traverseDist;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (11 * 4)], &traverseDist, sizeof(float));
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (23 * 2)], &traverseDistInt, sizeof(short));

			int null = 0;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (4 * 4)], &null, sizeof(int)); // mLinkData

			// Might as well null out some other things
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (2 * 4)], &null, sizeof(int)); // mData
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (3 * 4)], &null, sizeof(int)); // mProc
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (0 * 4)], &null, sizeof(int)); // idk what this is

			// Oh, and the HashId as well
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (7 * 4)], &null, sizeof(int));


			// Set name!
			std::string name = keyCodeMapIter->second;

			{ // Copy to name string storage... reversed
				int pos = sizeof(data.name) - 1;
				for (char const& c : name) {
					memcpy(data.name + pos, &c, 1);
					pos--;
				}
			}

			// -------------------------------------------------

			// Set registers for params and stuff
			data.n_r3 = data.f_r3;
			data.n_r4 = startData + sizeof(data) - sizeof(data.name);
			data.n_r5 = data.f_r5;
			data.n_r6 = actorStorageLocation;
			data.n_r7 = handleLocation;
			data.n_r8 = 0;
			data.n_r9 = 1;
			data.n_r10 = 0;

			data.fnAddr = 0x037b6040; // Address to call to

			data.enabled = true; // This tells the assembly patch to trigger one function call

			data.interceptRegisters = false; // We don't want to intercept *this* function call
		}

		{ // I feel like creating scope today
			std::map<char, bool>::iterator itr;
			itr = prevKeyStateMap.find(keyCodeMapIter->first);
			itr->second = keyPressed; // Key press logic
		}
	}

	memInstance->memory_writeMemoryBE(startData, data);
}


void init() {
    osLib_registerHLEFunctionType osLib_registerHLEFunction = (osLib_registerHLEFunctionType)GetProcAddress(GetModuleHandleA("Cemu.exe"), "osLib_registerHLEFunction");
	osLib_registerHLEFunction("coreinit", "fnCallMain", &mainFn); // Give our assembly patch something to hook into
}

/// <summary>
/// Manages getting console input - TEMPORARY while UI is still being developed
/// </summary>
DWORD WINAPI ConsoleThread(LPVOID param) {
	Console::ConsoleInit("SpawnActors Console");
	while (true) {
		std::string line = Console::ReadLine();
		if (line.size() == 0)
			continue;

		std::vector<std::string> command;
		{
			std::istringstream ss(line);
			std::string word;

			while (ss >> word)
			{
				command.push_back(word);
			}
		}
		std::transform(command[0].begin(), command[0].end(), command[0].begin(), // Make the command string lowercase
			[](unsigned char c) { return std::tolower(c); });

		if (command[0] == "help") {
			Console::LogPrint(
				"Commands:\n"
				"'help' - Shows commands\n"
				"'keycode [key] [actorname]' - Registers keycode for actor spawning\n"
				"'rmkeycode [key]' - Unregisters keycode for actor spawning"
			);
		}
		else if (command[0] == "keycode") {
			if (command.size() == 3) {
				keyCodeMap.insert({ std::toupper(command[1][0]), command[2] });
				prevKeyStateMap.insert({ std::toupper(command[1][0]), false });
			}
			else {
				Console::LogPrint("Format: keycode [key] [actorname]");
			}
		}
		else if (command[0] == "rmkeycode") {
			if (command.size() == 2) {
				keyCodeMap.erase(std::toupper(command[1][0]));
				prevKeyStateMap.erase(std::toupper(command[1][0]));
			}
			else {
				Console::LogPrint("Format: rmkeycode [key]");
			}
		}
	}
	return 0;
}


// Main DLL entrypoint
// -------------------
// By that, I just mean the stuff that gets called when the
// DLL is loaded or unloaded. The entrypoint from our assembly patch is above.
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		// Just include some niceties.
		// Not required, but good to have nonetheless.
		memInstance = new MemoryInstance(GetModuleHandleA(NULL));
		
		// This one is important - sets stuff up so that we can be called by the asm patch
        init();

		// Set up our console thread
		CreateThread(0, 0, ConsoleThread, hModule, 0, 0); // This isn't migrated to Threads because it's temporary

		CreateThread(0, 0, Threads::UIThread, hModule, 0, 0);
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		Console::ConsoleDealloc();
        break;
    }
    return TRUE;
}

