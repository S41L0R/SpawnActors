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
	int n_r10; // We could use a loop to loop through and read elements into the individual
	int n_r9; // vars in this struct as big endian, but this might be faster.. Also it's easier.
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


MemoryInstance* memInstance;
bool prevState = false;

void mainFn(PPCInterpreter_t* hCPU) {
	hCPU->instructionPointer = hCPU->sprNew.LR; // Tell it where to return to

	uint32_t startData = hCPU->gpr[3]; // Find where data starts from r3

	Data data;

	memInstance->memory_readMemoryBE(startData, &data);

	// Stuff goes here - this is gonna be called pretty often, you might wanna implement some key check stuff
	data.n_r3 = 0x400e80b0;
	data.n_r4 = 0x403325e0;
	data.n_r5 = 0xffffffff;
	data.n_r6 = 0x00000000;
	data.n_r7 = 0x00000000;
	data.n_r8 = 0x403325e0;
	data.n_r9 = 0x00000c00;
	data.n_r10 = 0x00000000;

	data.fnAddr = 0x037b6040;
	

	// Basic key press logic to make sure holding down doesn't spam triggers
	bool keyPressed = false;
	if (GetKeyState('Z') & 0x8000)
		keyPressed = true;

	if (keyPressed && !prevState)
		data.enabled = true;

	prevState = keyPressed;


	memInstance->memory_writeMemoryBE(startData, data);
}


void init() {
    osLib_registerHLEFunctionType osLib_registerHLEFunction = (osLib_registerHLEFunctionType)GetProcAddress(GetModuleHandleA("Cemu.exe"), "osLib_registerHLEFunction");
	osLib_registerHLEFunction("coreinit", "fnCallMain", &mainFn);
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		
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

