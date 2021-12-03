// dllmain.cpp : Defines the entry point for the DLL application.
#include "util/BotwEdit.h"




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
	char name[32];
	double str_27; // |
	double str_26; // |
	double str_25; // |
	double str_24; // |
	double str_23; // |
	double str_22; // |
	double str_21; // |
	double str_20; // |
	double str_19; // |
	double str_18; // |
	double str_17; // |
	double str_16; // |
	double str_15; // | This here is just general storage.
	double str_14; // | We can label and type it however we want, though.
	double str_13; // | We just have to make sure that the amount of bytes is consistant.
	double str_12; // |
	double str_11; // |
	double str_10; // |
	double str_9;  // |
	double str_8;  // |
	double str_7;  // |
	double str_6;  // |
	double str_5;  // |
	double str_4;  // |
	double str_3;  // |
	double str_2;  // |
	double str_1;  // |
	double str_0;  // |

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

MemoryInstance* memInstance;
bool prevState = false; // Used for key press logic - keeps track of previous key state

bool setup = false;

MemoryInstance::floatBE oldX = NULL;
MemoryInstance::floatBE oldY = NULL;
MemoryInstance::floatBE oldZ = NULL;

void mainFn(PPCInterpreter_t* hCPU) {
	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to - REQUIRED

	if (!setup) {
		uint32_t linkPosOffset;
		memInstance->memory_readMemoryBE(0x11344418, &linkPosOffset); // Some random reference to link's position that seems to work.
		memInstance->linkData.PosX = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x50);
		memInstance->linkData.PosY = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x54);
		memInstance->linkData.PosZ = reinterpret_cast<MemoryInstance::floatBE*>(memInstance->baseAddr + linkPosOffset + 0x58);


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
	memInstance->memory_readMemoryBE(startData, &data); // Just make sure to intercept stuff..
	data.interceptRegisters = true;
	memInstance->memory_writeMemoryBE(startData, data);

	// Basic key press logic to make sure holding down doesn't spam triggers //
	bool keyPressed = false;                                                 //
	if (GetKeyState('Z') & 0x8000)                                           //
		keyPressed = true;                                                   //
	                                                                         //
	if (keyPressed && !prevState) { // --------------------------------------//

		uint32_t startData = hCPU->gpr[3]; // Find where data starts from r3


		// Lets set any data that our params will reference:
		// -------------------------------------------------
		memInstance->memory_readMemory(data.f_r6 + (7 * 4) + 0, &oldX); // Back up the old position
		memInstance->memory_readMemory(data.f_r6 + (7 * 4) + 4, &oldY);
		memInstance->memory_readMemory(data.f_r6 + (7 * 4) + 8, &oldZ);

		memInstance->memory_writeMemory(data.f_r6 + (7 * 4) + 0, *memInstance->linkData.PosX);
		memInstance->memory_writeMemory(data.f_r6 + (7 * 4) + 4, *memInstance->linkData.PosY);
		memInstance->memory_writeMemory(data.f_r6 + (7 * 4) + 8, *memInstance->linkData.PosZ);
		std::string boko = "Enemy_Bokoblin_Junior";

		{ // Copy to name string storage
			int pos = sizeof(data.name) - 1;
			for (char const& c : boko) {
				memcpy(data.name + pos, &c, 1);
				pos--;
			}
		}

		// -------------------------------------------------

		// Set registers for params and stuff
		data.n_r3 = data.f_r3;
		data.n_r4 = startData + sizeof(data) - sizeof(data.name);
		data.n_r5 = data.f_r5;
		data.n_r6 = data.f_r6;
		data.n_r7 = data.f_r7;
		data.n_r8 = data.f_r8;
		data.n_r9 = data.f_r9;
		data.n_r10 = data.f_r10;

		data.fnAddr = 0x037b6040; // Address to call to

		data.enabled = true; // This tells the assembly patch to trigger one function call

		memInstance->memory_writeMemoryBE(startData, data);
	}

	prevState = keyPressed; // Again, related to key press logic
}


void init() {
    osLib_registerHLEFunctionType osLib_registerHLEFunction = (osLib_registerHLEFunctionType)GetProcAddress(GetModuleHandleA("Cemu.exe"), "osLib_registerHLEFunction");
	osLib_registerHLEFunction("coreinit", "fnCallMain", &mainFn); // Give our assembly patch something to hook into
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
		DebugConsole::consoleInit();
        init();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

