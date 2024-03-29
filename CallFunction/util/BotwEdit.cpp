// BotwEdit.cpp : Defines the functions for the static library.
//

#include "BotwEdit.h"
#include <cstdint>
#include <stdint.h>
#include <Windows.h>
#include <array>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>


struct LinkData {
	float* PosX;
	float* PosY;
	float* PosZ;
};

MemoryInstance::MemoryInstance(HMODULE cemuModuleHandle)
{

	memory_getBaseType memory_getBase = (memory_getBaseType)GetProcAddress(cemuModuleHandle, "memory_getBase");

	baseAddr = (uint64_t)memory_getBase();
	/* We don't need this junk:

	// Init linkData
	uint32_t linkPosOffset;
	memory_readMemoryBE(0x11344418, &linkPosOffset); // Some random reference to link's position that seems to work.
	linkData.PosX = reinterpret_cast<floatBE*>(baseAddr + linkPosOffset + 0x50);
	linkData.PosY = reinterpret_cast<floatBE*>(baseAddr + linkPosOffset + 0x54);
	linkData.PosZ = reinterpret_cast<floatBE*>(baseAddr + linkPosOffset + 0x58);

	linkData.VelX = reinterpret_cast<floatBE*>(baseAddr + linkPosOffset + 0xC0);
	linkData.VelY = reinterpret_cast<floatBE*>(baseAddr + linkPosOffset + 0xC4);
	linkData.VelZ = reinterpret_cast<floatBE*>(baseAddr + linkPosOffset + 0xC8);
	
	linkData.Stamina = reinterpret_cast<floatBE*>(baseAddr + 0x41C132A8);
	// Again, dual location strat
	// Kind of bad, but it works for now
	floatBE staminaMax1;
	floatBE staminaMax2;
	memory_readMemory(0x43022CB8, &staminaMax1);
	memory_readMemory(0x430224DC, &staminaMax2);
	if ((float)staminaMax1 > (float)staminaMax2) {
		linkData.StaminaMax = reinterpret_cast<floatBE*>(baseAddr + 0x43022CB8);
	}
	else {
		linkData.StaminaMax = reinterpret_cast<floatBE*>(baseAddr + 0x430224DC);
	}
	linkData.ActionSpeed = reinterpret_cast<floatBE*>(baseAddr + 0x3F3B4B90);
	linkData.SoundSpeed = reinterpret_cast<floatBE*>(baseAddr + 0x3E11EFE4);

	uint8_t runSpeedAOB[] = { 66, 112, 0, 0, 66, 200, 0, 0, 68, 122, 0, 0 };

	linkData.RunSpeed = reinterpret_cast<floatBE*>(baseAddr + (memory_aobScan(runSpeedAOB, 0x10000000, 0x80000000) - 8));

	linkData.controlData.B_Held = reinterpret_cast<bool*>(baseAddr + 0x41BAC157);
	linkData.controlData.KeyComboCode = reinterpret_cast<intBE*>(baseAddr + 0x41BAC3C4);

	gameData.WorldSpeed = reinterpret_cast<floatBE*>(baseAddr + 0x3F3B4B84);
	gameData.WorldSoundSpeed = reinterpret_cast<floatBE*>(baseAddr + 0x3E11FFC4);

	gameData.TimeOfDay = reinterpret_cast<floatBE*>(baseAddr + 0xA00AF878);

	*/
}

void MemoryInstance::RuntimeInit() {
	// Health tends to appear in one of two locations.
	// TODO: Proper hook
	uint8_t health1;
	uint8_t health2;
	memory_readMemory(0x430216FB, &health1);
	memory_readMemory(0x43021ED7, &health2);
	if (health1 > health2) {
		linkData.Health = reinterpret_cast<uint8_t*>(baseAddr + 0x430216FB);
	}
	else {
		linkData.Health = reinterpret_cast<uint8_t*>(baseAddr + 0x43021ED7);
	}

	std::vector<int> coordAob = { 0xff, 0xff, 0xff, 0xf1, 0x3f, -1, -1, -1, 0x80, 0x00, -1, -1, 0x11, -1, -1, -1, 0x10, 0xdf, -1, -1, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80 };
	uint64_t coordAddr = memory_aobScan(coordAob);
	linkData.PosX = reinterpret_cast<floatBE*>(coordAddr + 0xb4 + 0x0);
	linkData.PosY = reinterpret_cast<floatBE*>(coordAddr + 0xb4 + 0x4);
	linkData.PosZ = reinterpret_cast<floatBE*>(coordAddr + 0xb4 + 0x8);
	std::stringstream stream;
	stream << std::hex << coordAddr;
	std::string result(stream.str());
	Console::LogPrint(result);
}





HANDLE DebugConsole::debugConsoleHandle = NULL;

void DebugConsole::ConsoleInit() {
#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleA("Debug Console");
	debugConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONOUT$", "w", stderr);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
#endif
}

void DebugConsole::ConsoleDealloc() {
#ifdef _DEBUG
	FreeConsole();
#endif
}

void DebugConsole::LogPrint(const std::string_view& message_view) {
#ifdef _DEBUG
	std::string message(message_view);
	message += "\n";
	DWORD charsWritten = 0;
	WriteConsoleA(debugConsoleHandle, message.c_str(), (DWORD)message.size(), &charsWritten, NULL);
	OutputDebugStringA(message.c_str());
#endif
}

void DebugConsole::LogPrint(const char* message) {
	std::string stringLine(message);
	LogPrint(stringLine);
}

void DebugConsole::LogPrint(const float messageFloat) {
	std::string stringLine = std::to_string(messageFloat);
	LogPrint(stringLine);
}

std::string DebugConsole::ReadLine() {
	std::string out;
	std::getline(std::cin, out);
	return out;
}

HANDLE Console::consoleHandle = NULL;

void Console::ConsoleInit(std::string title) {
	AllocConsole();
	SetConsoleTitleA(title.c_str());
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONOUT$", "w", stderr);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}

void Console::ConsoleDealloc() {
	FreeConsole();
}

void Console::LogPrint(const std::string_view& message_view) {
	std::string message(message_view);
	message += "\n";
	DWORD charsWritten = 0;
	WriteConsoleA(consoleHandle, message.c_str(), (DWORD)message.size(), &charsWritten, NULL);
	OutputDebugStringA(message.c_str());
}

void Console::LogPrint(const char* message) {
	std::string stringLine(message);
	LogPrint(stringLine);
}

void Console::LogPrint(const float messageFloat) {
	std::string stringLine = std::to_string(messageFloat);
	LogPrint(stringLine);
}

std::string Console::ReadLine() {
	std::string out;

	if (std::getline(std::cin, out))
		return out;
	else
		return "ERROR";
}
