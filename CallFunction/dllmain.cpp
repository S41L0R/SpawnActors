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



struct Data { // This is reversed compared to the gfx pack because we read as big endian.
	char name_31;  // We could use a loop to loop through and read elements into the individual
	char name_30;  // vars in this struct as big endian, but this might be faster.. Also it's easier.
	char name_29;  // |
	char name_28;  // |
	char name_27;  // |
	char name_26;  // |
	char name_25;  // |
	char name_24;  // |
	char name_23;  // |
	char name_22;  // |
	char name_21;  // |
	char name_20;  // |
	char name_19;  // |
	char name_18;  // |
	char name_17;  // |
	char name_16;  // |
	char name_15;  // |
	char name_14;  // |
	char name_13;  // |
	char name_12;  // |
	char name_11;  // |
	char name_10;  // |
	char name_9;   // |
	char name_8;   // |
	char name_7;   // |
	char name_6;   // |
	char name_5;   // |
	char name_4;   // |
	char name_3;   // |
	char name_2;   // |
	char name_1;   // |
	char name_0;   // |
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
	int padding; // This is just padding - sometimes the struct can have an odd number of things, and I've spent the last 3 hours debugging this stupid stupid compiler argghghggggg

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
	int enabled;
};

// ---------------------------------------------------------------------------------
// This is an example function call.
// - Feel free to expand / change -
// Note: This does not take into account stuff like actually setting desired params.
// ---------------------------------------------------------------------------------

MemoryInstance* memInstance;
bool prevState = false; // Used for key press logic - keeps track of previous key state

void mainFn(PPCInterpreter_t* hCPU) {
	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to - REQUIRED


	// Basic key press logic to make sure holding down doesn't spam triggers //
	bool keyPressed = false;                                                 //
	if (GetKeyState('Z') & 0x8000)                                           //
		keyPressed = true;                                                   //
	                                                                         //
	if (keyPressed && !prevState) { // --------------------------------------//

		uint32_t startData = hCPU->gpr[3]; // Find where data starts from r3

		Data data;
		memInstance->memory_readMemoryBE(startData, &data);

		// Lets set any data that our params will reference:
		// -------------------------------------------------
		// The name:
		{
			data.name_0 = 'T';
			data.name_1 = 'e';
			data.name_2 = 's';
			data.name_3 = 't';
			data.name_4 = '\0'; // Null termination is important.
		}
		

		// -------------------------------------------------

		// Set registers for params and stuff
		data.n_r3 = 0x400e80b0;
		//data.n_r4 = 0x403325e0;
		data.n_r4 = startData + sizeof(data) - 32 - 0; // Just the location in memory for where we started our string earlier. 
		data.n_r5 = 0xffffffff;                        // I don't know of a better way to do this other than hardcoding values.
		data.n_r6 = 0x00000000;
		data.n_r7 = 0x00000000;
		data.n_r8 = 0x403325e0;
		data.n_r9 = 0x00000c00;
		data.n_r10 = 0x00000000;

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

