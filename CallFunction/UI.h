#pragma once

#include <shared_mutex>
#include <map>
#include <vector>

#include "util/BotwEdit.h"
#include "TriggeredActor.h"

#include "Windows.h"

#define NOMINMAX // Because cpp is stupid
#undef min // Because the last thing doesn't work.
#undef max // Cpp is stupid.

namespace Threads {
	
	DWORD WINAPI UIThread(LPVOID param);
}

namespace UIProcessor {
	extern std::map<char, std::vector<TriggeredActor>>* keyCodeMap;
	extern std::map<char, bool>* prevKeyStateMap;
	extern std::vector<TriggeredActor>* damageActors;
	extern MemoryInstance::floatBE* xPlayerPos;
	extern MemoryInstance::floatBE* yPlayerPos;
	extern MemoryInstance::floatBE* zPlayerPos;
	
	void WriteSettings();
	void LoadSettings();
}