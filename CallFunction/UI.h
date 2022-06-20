#pragma once

#include <shared_mutex>
#include <map>
#include <vector>

#include "util/BotwEdit.h"
#include "KeyCodeActor.h"

#include "Windows.h"

namespace Threads {
	
	DWORD WINAPI UIThread(LPVOID param);
}

namespace UIProcessor {
	extern std::map<char, std::vector<TriggeredActor>>* keyCodeMap;
	extern std::map<char, bool>* prevKeyStateMap;
	extern std::vector<TriggeredActor>* damageActors;
	
	void WriteSettings();
	void LoadSettings();
}