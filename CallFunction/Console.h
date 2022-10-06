#pragma once

#include <shared_mutex>
#include <map>
#include <vector>

#include "util/BotwEdit.h"
#include "TriggeredActor.h"

#include "Windows.h"

namespace Threads {
	DWORD WINAPI ConsoleThread(LPVOID param);
}

namespace ConsoleProcessor {
	void registerPresetKeycodes();

	extern MemoryInstance* memInstance;
	extern std::shared_mutex* keycode_mutex;
	extern std::map<char, std::vector<TriggeredActor>>* keyCodeMap;
	extern std::map<char, bool>* prevKeyStateMap;
	extern bool* isSetup;
}