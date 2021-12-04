#pragma once
#include <cstdint>
#include <stdint.h>
#include <Windows.h>
#include <array>
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

class MemoryInstance {
public:
	MemoryInstance(HMODULE cemuModuleHandle);

	template <typename T>
	static void memory_swapEndianness(T& val) {
		union U {
			T val;
			std::array<std::uint8_t, sizeof(T)> raw;
		} src, dst;

		memcpy(&src.val, &val, sizeof(val));
		std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
		memcpy(&val, &dst.val, sizeof(val));
	}

	template <typename T>
	void memory_readMemory(uint64_t offset, T* outPtr) {
		memcpy(outPtr, (void*)(offset + baseAddr), sizeof(T));
	}

	template <typename T>
	void memory_readMemoryBE(uint64_t offset, T* outPtr) {
		memcpy(outPtr, (void*)(offset + baseAddr), sizeof(T));
		memory_swapEndianness(*outPtr);
	}

	template <typename T>
	void memory_writeMemory(uint64_t offset, T value) {
		memcpy((void*)(offset + baseAddr), (void*)&value, sizeof(T));
	}

	template <typename T>
	void memory_writeMemoryBE(uint64_t offset, T value) {
		memory_swapEndianness(value);
		memcpy((void*)(offset + baseAddr), (void*)&value, sizeof(T));
	}
	
	template <size_t L>
	size_t memory_aobScan(uint8_t(&scanData)[L], size_t startOffset, size_t endOffset) {
		size_t endAddr = baseAddr + endOffset;
		int thing = sizeof(scanData);
		for (int addr = startOffset; addr < endOffset; addr = addr + 0x1) {
			uint8_t memorySegment[sizeof(scanData)];
			memory_readMemory(addr, &memorySegment);
			bool matches = true;
			for (int i = 0; i < sizeof(scanData); i++) {
				if (memorySegment[i] != scanData[i]) {
					matches = false;
					break;
				}
			}
			if (matches) {
				return addr;
			}
		}
		return 0;
	}
	
	/*
	template <size_t L>
	size_t memory_aobScan(uint8_t(&scanData)[L]) {
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		//DWORD procMin = (DWORD)sysInfo.lpMinimumApplicationAddress;
		//DWORD procMax = (DWORD)sysInfo.lpMaximumApplicationAddress;

		DWORD procMin = baseAddr + 0x10000000;
		DWORD procMax = baseAddr + 0x40000000;

		MEMORY_BASIC_INFORMATION mbi;

		while (procMin < procMax) {
			if (VirtualQuery((LPVOID)procMin, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != 0 && mbi.State == MEM_COMMIT && (mbi.Type == MEM_MAPPED || mbi.Type == MEM_PRIVATE)) {

				for (auto addr = (DWORD)mbi.BaseAddress; addr + sizeof(scanData) - 1 < (DWORD)mbi.BaseAddress + mbi.RegionSize; addr += 0x1) {
					uint8_t memorySegment[sizeof(scanData)];
					memcpy(&memorySegment, (void*)(addr), sizeof(scanData));
					bool matches = true;
					for (int i = 0; i < sizeof(scanData); i++) {
						if (memorySegment[i] != scanData[i]) {
							matches = false;
							break;
						}
					}
					if (matches) {
						return addr - baseAddr;
					}
				}

			}
			procMin += mbi.RegionSize;
		}
	}
	*/

	template <typename T>
	DWORD GetAddressFromSignature(std::vector<T> signature, DWORD startaddress = 0, DWORD endaddress = 0) {
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		if (startaddress == 0) {
			startaddress = (DWORD)(si.lpMinimumApplicationAddress);
		}
		else {
			startaddress = baseAddr + startaddress;
		}
		if (endaddress == 0) {
			endaddress = (DWORD)(si.lpMaximumApplicationAddress);
		}
		else {
			endaddress = baseAddr + endaddress;
		}

		MEMORY_BASIC_INFORMATION mbi{ 0 };
		DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

		for (DWORD i = startaddress; i < endaddress - signature.size(); i++) {
			//std::cout << "scanning: " << std::hex << i << std::endl;
			if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
				if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
					//std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
					i += mbi.RegionSize;
					continue; // if bad address then dont read from it
				}
				//std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
				for (DWORD k = (DWORD)mbi.BaseAddress; k < (DWORD)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++) {
					for (DWORD j = 0; j < signature.size(); j++) {
						if (signature.at(j) != -1 && signature.at(j) != *(uint8_t*)(k + j)) {
							break;
						}
						if (j + 1 == signature.size()) {
							return k - baseAddr;
						}
					}
				}
				i = (DWORD)mbi.BaseAddress + mbi.RegionSize;
			}
		}
		return NULL;
	}

	static class floatBE {
	public:
		floatBE() {
			data = 0.f;
		}
		floatBE(float input)
		{
			float copy = input;
			memory_swapEndianness(copy);
			data = copy;
		}

		void operator = (const float val) {
			float copy = val;
			memory_swapEndianness(copy);
			data = copy;
		}
		void operator = (const floatBE val) {
			data = val;
		}
		floatBE operator * (float val) {
			float dataCopy = data;
			memory_swapEndianness(dataCopy);
			dataCopy = dataCopy * val;
			return(floatBE(dataCopy));
		}
		floatBE operator * (double val) {
			floatBE::operator *((float)val);
		}
		operator float() const {
			float copy = data;
			memory_swapEndianness(copy);
			return copy;
		}
	private:
		float data; // This is the big endian data, we're just using the float type because it has the right about of bytes.
	};

	static class intBE {
	public:
		void operator = (const int val) {
			data = val;
			memory_swapEndianness(data);
		}
		void operator = (const intBE val) {
			data = val;
		}
		operator int() const {
			float copy = data;
			memory_swapEndianness(copy);
			return copy;
		}
	private:
		int data; // This is the big endian data, we're just using the int type because it has the right about of bytes.
	};

	struct {
		floatBE* PosX;
		floatBE* PosY;
		floatBE* PosZ;
		floatBE* VelX;
		floatBE* VelY;
		floatBE* VelZ;
		uint8_t* Health;
		floatBE* Stamina;
		floatBE* StaminaMax;
		floatBE* ActionSpeed;
		floatBE* SoundSpeed;
		floatBE* RunSpeed;
		struct {
			const bool* B_Held;
			const intBE* KeyComboCode;
		} controlData;
		
		/*
		floatBE* CameraPosX() {

		}
		floatBE* CameraPosY() {

		}
		floatBE* CameraPosZ() {

		}
		glm::fmat3 cameraForwardVector() {

		}
		*/
	} linkData;

	struct {
		floatBE* WorldSpeed;
		floatBE* WorldSoundSpeed;
		floatBE* TimeOfDay;

	} gameData;

	uint64_t baseAddr;
private:

	typedef void* (*memory_getBaseType)();
};

namespace DebugConsole {
	void consoleInit();
	void ConsoleDealloc();
	void logPrint(const std::string_view& message_view);
	void logPrint(const char* message);
	void logPrint(const float messageFloat);
	std::string readLine();
	extern HANDLE debugConsoleHandle;
}

namespace Console {
	void consoleInit(std::string title);
	void ConsoleDealloc();
	void logPrint(const std::string_view& message_view);
	void logPrint(const char* message);
	void logPrint(const float messageFloat);
	std::string readLine();
	extern HANDLE consoleHandle;
}