// dllmain.cpp : Defines the entry point for the DLL application.
#include <map>
#include <iostream>
#include <fstream>
#include <shared_mutex>

#include "util/BotwEdit.h"
#include "UI.h"
#include "ActorData.h"

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
	char name[152]; // We'll allocate all unused storage for use for name storage.. just in case of a really long actor name
	uint8_t actorStorage[104];

	int f_r10;
	int f_r9;
	int f_r8;
	int f_r7;
	int f_r6;
	int f_r5;
	int f_r4;
	int f_r3;

	int fo_r0;

	int n_r10;
	int n_r9;
	int n_r8;
	int n_r7;
	int n_r6;
	int n_r5;
	int n_r4;
	int n_r3;

	int o_ctr;
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

	byte padding_0[2];
	bool interceptRegisters;

	bool enabled;
};

struct KeyCodeActor {
	KeyCodeActor(std::string name) {
		Name = name;
	}
	KeyCodeActor(std::string name, bool randomized) {
		Name = name;
		Randomized = randomized;
	}

	std::string Name;
	bool Randomized = false;
};

struct QueueActor {
	float PosX;
	float PosY;
	float PosZ;
	std::string Name;
};

// ---------------------------------------------------------------------------------
// This is an example function call.
// - Feel free to expand / change -
// Note: This does not take into account stuff like actually setting desired params.
// ---------------------------------------------------------------------------------
std::map<char, std::vector<KeyCodeActor>> keyCodeMap;

std::shared_mutex mutex; // Make this thread-safe.

std::map<char, bool> prevKeyStateMap; // Used for key press logic - keeps track of previous key state

std::vector<QueueActor> queuedActors;


MemoryInstance* memInstance;

bool setup = false;

// Trying to make some stuff from the MemEditor Decomp work for us ig
int findSequenceMatch(char array[], int start, char searchSequence[], bool loop = true, bool debug = false) {
	int num = strlen(array);
	int num2 = searchSequence[0];
	while (start < num){
		if (num2 == -1 || (num2 == -2 && array[start] != 0) || (num2 > -1 && array[start] == (byte)num2)){
			for (int i = 1; i <= strlen(searchSequence); i++){
				if (i >= strlen(searchSequence)){
					return start;
				}
				if (i > 19){
					int num3 = searchSequence[i];
					"0x" + std::to_string(num3);
					int num4 = (int)array[start + i];
					"0x" + std::to_string(num4);
				}
				if (searchSequence[i] != -1 && (searchSequence[i] != -2 || array[start + i] == 0)){
					if (array[start + i] != (byte)searchSequence[i]){
						break;
					}
					if (i == strlen(searchSequence) + i){
						return start;
					}
				}
			}
		}
		if (!loop){
			break;
		}
		start++;
	}
	return -1;
}

long pagedMemorySearchMatch(int search[], long startAddress, long regionSize, int processId){
	long result = -1L;
	int val = 20480;
	int num = std::max((strlen(search) * 20), val);
	if (processId == NULL){
		return result;
	}
	//intptr_t hProcess =
	byte array[] = new byte[num];
	long num2 = startAddress + regionSize;
	for (long num3 = startAddress; num3 < num2; num3 += (long)(strlen(array) - strlen(search))){

	}
}

void mainFn(PPCInterpreter_t* hCPU) {


	if (!setup) {
		uint32_t linkPosOffset = 0x113444F0;
		memInstance->linkData.PosX = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x50); // oh wait,
		memInstance->linkData.PosY = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x54); // it's
		memInstance->linkData.PosZ = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x58); // inconsistent

		// Realistically, no one's gonna be at *exactly* 0 0 0
		if (*memInstance->linkData.PosX == 0.f && *memInstance->linkData.PosY == 0.f && *memInstance->linkData.PosZ == 0.f) {
			Console::LogPrint("Whelp... 0 0 0 Glitch. Restart cemu and try again!");
			Console::LogPrint("I really need to figure out why this happens...");
			Console::LogPrint("I could do what LibreVR's Memory Editor does, which is an AOB scan, but the problems I have with that are:");
			Console::LogPrint("  A. It's really slow");
			Console::LogPrint("  B. It requires some complicated region finding");
		}

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
	int size = sizeof(data);
	memInstance->memory_readMemoryBE(startData, &data);

	data.interceptRegisters = true; // Just make sure to intercept stuff.. if we don't do this all the time when you warp somewhere else spawns cause it to crash


	// Basic key press logic to make sure holding down doesn't spam triggers
	mutex.lock();
	for (std::map<char, std::vector<KeyCodeActor>>::iterator keyCodeMapIter = keyCodeMap.begin(); keyCodeMapIter != keyCodeMap.end(); ++keyCodeMapIter) {
		char key = keyCodeMapIter->first;

		bool keyPressed = false;
		if (GetKeyState(key) & 0x8000)
			keyPressed = true;

		if (keyPressed && !prevKeyStateMap.find(keyCodeMapIter->first)->second) { // Make sure the key is pressed this frame and wasn't last frame
			for (KeyCodeActor keyCodeActor : keyCodeMapIter->second) {
				QueueActor queueActor;
				// Set name
				queueActor.Name = keyCodeActor.Name; // Default
				if (keyCodeActor.Randomized) {
					int largestAcceptedNameLength = 0;
					for (std::map<std::string, ActorData::Enemy>::iterator iter = ActorData::EnemyClasses.begin(); iter != ActorData::EnemyClasses.end(); ++iter) {
						if (keyCodeActor.Name.find(iter->first, 0) == 0) {

							if (iter->first.length() < largestAcceptedNameLength) // Some optimizations
								continue;
							largestAcceptedNameLength = iter->first.length();

							std::string name = iter->first;
							std::string variant = iter->second.Variants.at(std::rand() % iter->second.Variants.size());
							if (variant != "") {
								name.append("_");
								name.append(variant);
							}

							//for (int i = 0; i <= iter->second.MaxWeaponSlots; i++) {

							//}
							queueActor.Name = name;
						}
					}

					largestAcceptedNameLength = 0;
					for (std::map<std::string, ActorData::Weapon>::iterator iter = ActorData::WeaponClasses.begin(); iter != ActorData::WeaponClasses.end(); ++iter) {
						if (keyCodeActor.Name.find(iter->first, 0) == 0) {

							if (iter->first.length() < largestAcceptedNameLength) // Some optimizations
								continue;
							largestAcceptedNameLength = iter->first.length();

							std::string name = iter->first;
							name.append("_");
							name.append(iter->second.Variants.at(std::rand() % iter->second.Variants.size()));

							queueActor.Name = name;
						}
					}
				}

				queueActor.PosX = (float)*memInstance->linkData.PosX;
				queueActor.PosY = (float)*memInstance->linkData.PosY + 3; // A bit of an offset so stuff (especially weapons) doesn't spawn underground
				queueActor.PosZ = (float)*memInstance->linkData.PosZ;

				queuedActors.push_back(queueActor);
			}
		}


		// Actual actor spawning - just read from queue here.
		if (queuedActors.size() >= 1) {
			QueueActor qAct = queuedActors[0];

			uint32_t startData = hCPU->gpr[3]; // Find where data starts from r3


			// Lets set any data that our params will reference:
			// -------------------------------------------------

			// Copy needed data over to our own storage to not override actor stuff
			memInstance->memory_readMemoryBE(data.f_r7, &data.actorStorage);
			int actorStorageLocation = startData + sizeof(data) - sizeof(data.name) - sizeof(data.actorStorage);
			int mubinLocation = startData + sizeof(data) - sizeof(data.name) - sizeof(data.actorStorage) + (7 * 4); // The MubinIter lives inside the actor btw

			// Set actor pos to stored pos
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (15 * 4)], &qAct.PosX, sizeof(float));
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (16 * 4)], &qAct.PosY, sizeof(float));
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (17 * 4)], &qAct.PosZ, sizeof(float));

			// We want to make sure there's a fairly high traverseDist
			float traverseDist = 0.f; // Hmm... this kinda proves this isn't really used
			short traverseDistInt = (short)traverseDist;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (18 * 4)], &traverseDist, sizeof(float));
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (37 * 2)], &traverseDistInt, sizeof(short));

			int null = 0;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (11 * 4)], &null, sizeof(int)); // mLinkData

			// Might as well null out some other things
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (9 * 4)], &null, sizeof(int)); // mData
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (10 * 4)], &null, sizeof(int)); // mProc
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (7 * 4)], &null, sizeof(int)); // idk what this is
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (3 * 4)], &null, sizeof(int)); // or this, either



			// Not sure what these are, but they helps with traverseDist issues
			int traverseDistFixer = 0x043B0000;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (2 * 4)], &traverseDistFixer, sizeof(int));
			int traverseDistFixer2 = 0x00000016;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (1 * 4)], &traverseDistFixer2, sizeof(int));


			// Oh, and the HashId as well
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (14 * 4)], &null, sizeof(int));

			// And we can make mRevivalGameDataFlagHash an invalid handle
			int invalid = -1;
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (12 * 4)], &invalid, sizeof(int));
			// And whatever this is, too
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (13 * 4)], &invalid, sizeof(int));

			// We can also get rid of this junk
			memcpy(&data.actorStorage[sizeof(data.actorStorage) - (8 * 4)], &invalid, sizeof(int));




			// Set name!
			{ // Copy to name string storage... reversed
				int pos = sizeof(data.name) - 1;
				for (char const& c : qAct.Name) {
					memcpy(data.name + pos, &c, 1);
					pos--;
				}
				uint8_t nullByte = 0;
				memcpy(data.name + pos, &nullByte, 1); // Null terminate!
			}

			// -------------------------------------------------

			// Set registers for params and stuff
			data.n_r3 = data.f_r3;
			data.n_r4 = startData + sizeof(data) - sizeof(data.name);
			data.n_r5 = data.f_r5;
			data.n_r6 = mubinLocation;
			data.n_r7 = actorStorageLocation;
			data.n_r8 = 0;
			data.n_r9 = 1;
			data.n_r10 = 0;

			data.fnAddr = 0x037b6040; // Address to call to

			data.enabled = true; // This tells the assembly patch to trigger one function call

			data.interceptRegisters = false; // We don't want to intercept *this* function call

			// Gotta remove this actor from the queue!
			queuedActors.erase(queuedActors.begin());
		}

		{ // I feel like creating scope today
			std::map<char, bool>::iterator itr;
			itr = prevKeyStateMap.find(keyCodeMapIter->first);
			itr->second = keyPressed; // Key press logic
		}
	}
	mutex.unlock();

	memInstance->memory_writeMemoryBE(startData, data);

	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to - REQUIRED
}


void init() {
    osLib_registerHLEFunctionType osLib_registerHLEFunction = (osLib_registerHLEFunctionType)GetProcAddress(GetModuleHandleA("Cemu.exe"), "osLib_registerHLEFunction");
	osLib_registerHLEFunction("coreinit", "fnCallMain", &mainFn); // Give our assembly patch something to hook into
}

/// <summary>
/// Manages getting console input - TEMPORARY while UI is still being developed
/// </summary>
DWORD WINAPI ConsoleThread(LPVOID param) {
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
				"'keycode [key] [actorname(s)]' - Registers keycode for actor spawning\n"
				"'rmkeycode [key]' - Unregisters keycode for actor spawning\n"
				"'pos' - Print link's pos"
			);
		}
		else if (command[0] == "keycode") {
			mutex.lock();
			if (command.size() >= 3) {
				std::vector<KeyCodeActor> actVec;
				int actorCount = command.size() - 2;
				for (int i = 0; i < actorCount; i++) {
					bool randomized = (command[i + 2][0] == '\\');
					if (randomized)
						command[i + 2].erase(0, 1);

					actVec.push_back(KeyCodeActor(command[i + 2], randomized));
				}
				if (keyCodeMap.find(command[1][0]) != keyCodeMap.end()) // Remove last version if it exists
					keyCodeMap.erase(keyCodeMap.find(command[1][0]));

				keyCodeMap.insert({ std::toupper(command[1][0]), actVec });
				prevKeyStateMap.insert({ std::toupper(command[1][0]), false });
				Console::LogPrint("Keycode added succesfully");
			}
			else {
				Console::LogPrint("Format: keycode [key] [actorname(s)]");
			}
			mutex.unlock();
		}
		else if (command[0] == "rmkeycode") {
			mutex.lock();
			if (command.size() == 2) {
				keyCodeMap.erase(std::toupper(command[1][0]));
				prevKeyStateMap.erase(std::toupper(command[1][0]));
				Console::LogPrint("Keycode Removed Succesfully");
			}
			else {
				Console::LogPrint("Format: rmkeycode [key]");
			}
			mutex.unlock();
		}
		else if (command[0] == "pos" && init) {
			Console::LogPrint(*memInstance->linkData.PosX);
			Console::LogPrint(*memInstance->linkData.PosY);
			Console::LogPrint(*memInstance->linkData.PosZ);
		}
	}
	return 0;
}

void registerPresetKeycodes() {
	std::ifstream txtFile;
	txtFile.open("keycodes.txt");

	std::string line;
	while (std::getline(txtFile, line)) {

		// Get our command vector
		std::vector<std::string> command;
		{
			std::istringstream ss(line);
			std::string word;

			while (ss >> word)
			{
				command.push_back(word);
			}
		}

		// Actually set the keycode!
		mutex.lock();
		if (command.size() >= 2) {
			std::vector<KeyCodeActor> actVec;
			int actorCount = command.size() - 1;
			for (int i = 0; i < actorCount; i++) {
				bool randomized = (command[i + 1][0] == '\\');
				if (randomized)
					command[i + 1].erase(0, 1);
				actVec.push_back(KeyCodeActor(command[i + 1], randomized));
			}
			if (keyCodeMap.find(command[0][0]) != keyCodeMap.end()) // Remove last version if it exists
				keyCodeMap.erase(keyCodeMap.find(command[0][0]));

			keyCodeMap.insert({ std::toupper(command[0][0]), actVec });
			prevKeyStateMap.insert({ std::toupper(command[0][0]), false });
			Console::LogPrint("Keycode added succesfully");
		}
		mutex.unlock();
	}
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

		Console::ConsoleInit("SpawnActors Console"); // Set up our console

		// This one is important - sets stuff up so that we can be called by the asm patch
        init();

		// And set up ActorData
		ActorData::InitDefaultValues();

		// Set up keycodes set by file
		registerPresetKeycodes();

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

