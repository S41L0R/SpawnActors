#include <Windows.h>
#include <TlHelp32.h>
#include <filesystem>
#include <iostream>


DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);


	if (hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnapshot, &procEntry)) {
			do {
				if (!_stricmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &procEntry));
		}
	}
	CloseHandle(hSnapshot);
	return procId;
}


std::string GetCurrentDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	return std::string(buffer).substr(0, pos);
}

int main() {
	// Get the injector's path and set it as the current path
	// Thanks to https://gist.github.com/karolisjan/f9b8ac3ae2d41ec0ce70f2feac6bdfaf
	std::filesystem::current_path(std::filesystem::path(GetCurrentDirectory()));

	std::string dllPathString = std::filesystem::absolute(std::filesystem::path("CallFunction.dll")).string();
	const char* dllPath = dllPathString.c_str();

	const char* procName = "Cemu.exe";
	DWORD procId = 0;

	while (!procId) {
		procId = GetProcId(procName);
		Sleep(30);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

	if (hProc && hProc != INVALID_HANDLE_VALUE) {
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (loc) {
			WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
		}

		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

		if (hThread) {
			CloseHandle(hThread);
		}

		if (hProc) {
			CloseHandle(hProc);
		}

		return 0;
	}
}