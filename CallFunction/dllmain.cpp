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
struct TransferableData { // This is reversed compared to the gfx pack because we read as big endian.
	int f_r10;
	int f_r9;
	int f_r8;
	int f_r7;
	int f_r6;
	int f_r5;
	int f_r4;
	int f_r3;

	int ringPtr;

	int fnAddr;

	byte bytepadding[2];
	bool interceptRegisters;

	bool enabled;
};

#pragma pack(1)
struct InstanceData {
	char name[152]; // We'll allocate all unused storage for use for name storage.. just in case of a really long actor name
	uint8_t actorStorage[104];
};

struct KeyCodeActor {
	KeyCodeActor(std::string name) {
		Name = name;
	}
	KeyCodeActor(std::string name, int num, bool randomized) {
		Name = name;
		Num = num;
		Randomized = randomized;
	}

	std::string Name;
	int Num = 1;
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

std::shared_mutex keycode_mutex;
std::shared_mutex queue_mutex;
std::shared_mutex data_mutex;

std::map<char, bool> prevKeyStateMap; // Used for key press logic - keeps track of previous key state

std::vector<QueueActor> queuedActors;


MemoryInstance* memInstance;

bool isSetup = false;

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

/*
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
*/

void logFn(PPCInterpreter_t* hCPU) {
	hCPU->instructionPointer = hCPU->sprNew.LR;

	Console::LogPrint((char*)(memInstance->baseAddr + hCPU->gpr[3]));
}

void setup(PPCInterpreter_t* hCPU, uint32_t startTrnsData) {
	Console::LogPrint("It looks like you've correctly set up SpawnActors.");

	uint32_t linkPosOffset = 0x113444F0;
	memInstance->linkData.PosX = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x50); // oh wait,
	memInstance->linkData.PosY = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x54); // it's
	memInstance->linkData.PosZ = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x58); // inconsistent

	// Realistically, no one's gonna be at *exactly* 0 0 0
	if (*memInstance->linkData.PosX == 0.f && *memInstance->linkData.PosY == 0.f && *memInstance->linkData.PosZ == 0.f) {
		Console::LogPrint(""); // New line
		Console::LogPrint("Whelp... 0 0 0 Glitch. Restart cemu and try again!");
		Console::LogPrint("I really need to figure out why this happens...");
		Console::LogPrint("I could do what LibreVR's Memory Editor does, which is an AOB scan, but the problems I have with that are:");
		Console::LogPrint("  A. It's really slow");
		Console::LogPrint("  B. It requires some complicated region finding");
		Console::LogPrint("What we'll probably end up doing is hooking into the coord init instructions, but we need to find that first.");
	}

	TransferableData trnsData;
	data_mutex.lock(); //////////////////////////////////////////////////
	memInstance->memory_readMemoryBE(startTrnsData, &trnsData); // Just make sure to intercept stuff..
	trnsData.interceptRegisters = true;
	memInstance->memory_writeMemoryBE(startTrnsData, trnsData);
	data_mutex.unlock(); //===============================================

	isSetup = true;
}

void queueActors() {
	keycode_mutex.lock();
	// Basic key press logic to make sure holding down doesn't spam triggers
	for (std::map<char, std::vector<KeyCodeActor>>::iterator keyCodeMapIter = keyCodeMap.begin(); keyCodeMapIter != keyCodeMap.end(); ++keyCodeMapIter) {
		char key = keyCodeMapIter->first;

		bool keyPressed = false;
		if (GetKeyState(key) & 0x8000)
			keyPressed = true;

		if (keyPressed && !prevKeyStateMap.find(keyCodeMapIter->first)->second) { // Make sure the key is pressed this frame and wasn't last frame
			for (KeyCodeActor keyCodeActor : keyCodeMapIter->second) {
				for (int i = 0; i < keyCodeActor.Num; i++) {
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

					queue_mutex.lock(); ////////////////////////////////////////
					queuedActors.push_back(queueActor);
					queue_mutex.unlock(); //====================================
				}
			}
		}
		{ // I feel like creating scope today
			std::map<char, bool>::iterator itr;
			itr = prevKeyStateMap.find(keyCodeMapIter->first);
			itr->second = keyPressed; // Key press logic
		}
	}
	keycode_mutex.unlock();
}

void setupActor(PPCInterpreter_t* hCPU, TransferableData& trnsData, InstanceData& instData, uint32_t startRingBuffer, uint32_t endRingBuffer) {
	QueueActor qAct = queuedActors[0];


	// Lets set any data that our params will reference:
	// -------------------------------------------------

	// Copy needed data over to our own storage to not override actor stuff
	data_mutex.lock_shared(); //////////////////////////////////////////////////
	memInstance->memory_readMemoryBE(trnsData.f_r7, &instData.actorStorage);
	data_mutex.unlock_shared(); //==============================================

	int actorStorageLocation = trnsData.ringPtr + sizeof(instData) - sizeof(instData.name) - sizeof(instData.actorStorage);
	int mubinLocation = trnsData.ringPtr + sizeof(instData) - sizeof(instData.name) - sizeof(instData.actorStorage) + (7 * 4); // The MubinIter lives inside the actor btw

	// Set actor pos to stored pos
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (15 * 4)], &qAct.PosX, sizeof(float));
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (16 * 4)], &qAct.PosY, sizeof(float));
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (17 * 4)], &qAct.PosZ, sizeof(float));

	// We want to make sure there's a fairly high traverseDist
	float traverseDist = 0.f; // Hmm... this kinda proves this isn't really used
	short traverseDistInt = (short)traverseDist;
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (18 * 4)], &traverseDist, sizeof(float));
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (37 * 2)], &traverseDistInt, sizeof(short));

	int null = 0;
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (11 * 4)], &null, sizeof(int)); // mLinkData

	// Might as well null out some other things
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (9 * 4)], &null, sizeof(int)); // mData
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (10 * 4)], &null, sizeof(int)); // mProc
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (7 * 4)], &null, sizeof(int)); // idk what this is
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (3 * 4)], &null, sizeof(int)); // or this, either



	// Not sure what these are, but they helps with traverseDist issues
	int traverseDistFixer = 0x043B0000;
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (2 * 4)], &traverseDistFixer, sizeof(int));
	int traverseDistFixer2 = 0x00000016;
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (1 * 4)], &traverseDistFixer2, sizeof(int));


	// Oh, and the HashId as well
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (14 * 4)], &null, sizeof(int));

	// And we can make mRevivalGameDataFlagHash an invalid handle
	int invalid = -1;
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (12 * 4)], &invalid, sizeof(int));
	// And whatever this is, too
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (13 * 4)], &invalid, sizeof(int));

	// We can also get rid of this junk
	memcpy(&instData.actorStorage[sizeof(instData.actorStorage) - (8 * 4)], &invalid, sizeof(int));




	// Set name!
	{ // Copy to name string storage... reversed
		int pos = sizeof(instData.name) - 1;
		for (char const& c : qAct.Name) {
			memcpy(instData.name + pos, &c, 1);
			pos--;
		}
		uint8_t nullByte = 0;
		memcpy(instData.name + pos, &nullByte, 1); // Null terminate!
	}

	// -------------------------------------------------

	// Set registers for params and stuff
	hCPU->gpr[3] = trnsData.f_r3;
	hCPU->gpr[4] = trnsData.ringPtr + sizeof(instData) - sizeof(instData.name);
	hCPU->gpr[5] = trnsData.f_r5;
	hCPU->gpr[6] = mubinLocation;
	hCPU->gpr[7] = actorStorageLocation;
	hCPU->gpr[8] = 0;
	hCPU->gpr[9] = 1;
	hCPU->gpr[10] = 0;
	trnsData.fnAddr = 0x037b6040; // Address to call to

	trnsData.enabled = true; // This tells the assembly patch to trigger one function call

	trnsData.interceptRegisters = false; // We don't want to intercept *this* function call

	// Write our actor data!
	data_mutex.lock(); ////////////////////////////////////////////////////
	memInstance->memory_writeMemoryBE(trnsData.ringPtr, instData);
	data_mutex.unlock(); //================================================

	trnsData.ringPtr += sizeof(InstanceData); // Move our ring ptr to the next slot!
	if (trnsData.ringPtr >= endRingBuffer) // If we're at the end of the ring....
		trnsData.ringPtr = startRingBuffer; // move to the start!

	// Gotta remove this actor from the queue!
	queuedActors.erase(queuedActors.begin());
}

void mainFn(PPCInterpreter_t* hCPU, uint32_t startTrnsData, uint32_t startRingBuffer, uint32_t endRingBuffer) {
	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to - REQUIRED

	// This is the stuff I'm currently using to test different values for potential Link coords
	//MemoryInstance::floatBE* pos = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + 0x10263910);
	//Console::LogPrint((float)*pos);

	if (!isSetup) {
		setup(hCPU, startTrnsData);
		return;
	}

	queueActors();

	data_mutex.lock_shared(); /////////////////////////////////////////////////////////////////////
	// Get our transferrable data
	TransferableData trnsData;
	memInstance->memory_readMemoryBE(startTrnsData, &trnsData);

	// Get our instance data
	uint32_t startInstData = trnsData.ringPtr;
	InstanceData instData;
	memInstance->memory_readMemoryBE(startInstData, &instData);
	data_mutex.unlock_shared(); //==================================================================

	trnsData.interceptRegisters = true; // Just make sure to intercept stuff.. if we don't do this all the time when you warp somewhere else spawns cause it to crash

	queue_mutex.lock(); ////////////////////////////////////////////
	// Actual actor spawning - just read from queue here.
	if (queuedActors.size() >= 1) {
		setupActor(hCPU, trnsData, instData, startRingBuffer, endRingBuffer);
	}
	queue_mutex.unlock(); //========================================

	data_mutex.lock(); ////////////////////////////////////////////////////////////////////
	memInstance->memory_writeMemoryBE(startTrnsData, trnsData);
	data_mutex.unlock(); //==================================================================
}

// A wrapper function to set the params in a more cpp-friendly way
void mainFn(PPCInterpreter_t* hCPU) {
	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to - REQUIRED
	mainFn(hCPU, hCPU->gpr[3], hCPU->gpr[4], hCPU->gpr[5]);
}


void init() {
    osLib_registerHLEFunctionType osLib_registerHLEFunction = (osLib_registerHLEFunctionType)GetProcAddress(GetModuleHandleA("Cemu.exe"), "osLib_registerHLEFunction");
	osLib_registerHLEFunction("spawnactors", "fnCallMain", static_cast<void (*) (PPCInterpreter_t*)>(&mainFn)); // Give our assembly patch something to hook into
	osLib_registerHLEFunction("spawnactors", "logFn", &logFn); // And basic logging tools
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
		keycode_mutex.lock();
		if (command.size() >= 3) {
			char keycode = std::toupper(command[0][0]);


			std::vector<KeyCodeActor> actVec;
			int actorCount = (command.size() - 1) / 2;

			for (int i = 0; i < command.size() - 1; i += 2) {
				int num = std::stoi(command[i + 1]);

				bool randomized = (command[i + 2][0] == '\\');
				if (randomized)
					command[i + 2].erase(0, 1);

				actVec.push_back(KeyCodeActor(command[i + 2], num, randomized));
			}

			if (keyCodeMap.find(keycode) != keyCodeMap.end()) // Remove last version if it exists
				keyCodeMap.erase(keyCodeMap.find(keycode));

			keyCodeMap.insert({ keycode, actVec });
			prevKeyStateMap.insert({ keycode, false });
			Console::LogPrint("Keycode added succesfully");
		}
		keycode_mutex.unlock();
	}
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
				"'keycode [key] [ [num] [actorname] ](s)' - Registers keycode for actor spawning\n"
				"'rmkeycode [key]' - Unregisters keycode for actor spawning\n"
				"'pos' - Print link's pos"
			);
		}
		else if (command[0] == "keycode") {
			keycode_mutex.lock();
			if (command.size() >= 4) {
				char keycode = std::toupper(command[1][0]);


				std::vector<KeyCodeActor> actVec;
				int actorCount = (command.size() - 2) / 2;

				for (int i = 0; i < command.size() - 2; i += 2) {
					int num = std::stoi(command[i + 2]);

					bool randomized = (command[i + 3][0] == '\\');
					if (randomized)
						command[i + 3].erase(0, 1);

					actVec.push_back(KeyCodeActor(command[i + 3], num, randomized));
				}


				if (keyCodeMap.find(keycode) != keyCodeMap.end()) // Remove last version if it exists
					keyCodeMap.erase(keyCodeMap.find(keycode));

				keyCodeMap.insert({ keycode, actVec });
				prevKeyStateMap.insert({ keycode, false });
				Console::LogPrint("Keycode added succesfully");
			}
			else {
				Console::LogPrint("Format: keycode [key] [actorname(s)]");
			}
			keycode_mutex.unlock();
		}
		else if (command[0] == "rmkeycode") {
			keycode_mutex.lock();
			if (command.size() == 2) {
				keyCodeMap.erase(std::toupper(command[1][0]));
				prevKeyStateMap.erase(std::toupper(command[1][0]));
				Console::LogPrint("Keycode Removed Succesfully");
			}
			else {
				Console::LogPrint("Format: rmkeycode [key]");
			}
			keycode_mutex.unlock();
		}
		else if (command[0] == "reloadconfig")
			registerPresetKeycodes();
		else if (command[0] == "pos" && init) {
			Console::LogPrint(*memInstance->linkData.PosX);
			Console::LogPrint(*memInstance->linkData.PosY);
			Console::LogPrint(*memInstance->linkData.PosZ);
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

		Console::ConsoleInit("SpawnActors Console"); // Set up our console

		// This one is important - sets stuff up so that we can be called by the asm patch
        init();

		// And set up ActorData
		ActorData::InitDefaultValues();

		// Set up keycodes set by file
		registerPresetKeycodes();

		// Set up our console thread
		CreateThread(0, 0, ConsoleThread, hModule, 0, 0); // This isn't migrated to Threads because it's temporary

		//CreateThread(0, 0, Threads::UIThread, hModule, 0, 0);
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

