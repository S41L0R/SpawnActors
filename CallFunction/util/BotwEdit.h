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

	void RuntimeInit();

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
	
	uint64_t memory_aobScan(std::vector<int> signature, int region = 0, uint64_t regionOffset = 0, bool multiple = false, bool multipleRegions = false, uint64_t regionMaxOffset = 0)
	{

		SYSTEM_INFO si;
		GetSystemInfo(&si);

		uint64_t startAddress = baseAddr;
		uint64_t endAddress = (uint64_t)(si.lpMaximumApplicationAddress);

		MEMORY_BASIC_INFORMATION mbi{ 0 };
		DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

		int contador = 1;
		for (uint64_t i = startAddress; i < endAddress - signature.size();) {
			if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
				if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
					i += mbi.RegionSize;

					contador++;
					continue; // if bad adress then dont read from it
				}

				if (region != 0 && contador < region)
				{
					i += mbi.RegionSize;
					contador++;
					continue;
				}

				if (contador > region && region != 0 && !multipleRegions)
				{
					break;
				}


				//get last '?' position in pattern and use it to calculate the max shift value.
				//the last position in the pattern should never be a '?' -> we do not bother checking it
				uint64_t maxShift = signature.size() - 1;
				uint64_t maxIndex = signature.size() - 2;
				uint64_t wildCardIndex = 0;
				for (uint64_t i = 0; i < maxIndex + 1; i++) {
					if (signature.at(i) == -1) {
						maxShift = maxIndex - i;
						wildCardIndex = i;
					}
				}

				//initialize the shift table
				uint64_t shiftTable[256];
				for (uint64_t i = 0; i <= 255; i++) {
					shiftTable[i] = maxShift;
				}


				//fill shiftTable
				//forgot this in the video: Because max shift should always be '?' we only update the shift table for bytes to the right of the last '?'
				for (uint64_t i = wildCardIndex + 1; i < maxIndex; i++) {
					shiftTable[signature.at(i)] = maxIndex - i;
				}


				uint64_t startingAddress = 0;
				uint64_t endAddress = mbi.RegionSize - signature.size();

				if (region != 0 && regionOffset != 0)
				{
					startingAddress = regionOffset;
				}

				if (region != 0 && regionMaxOffset != 0)
				{
					if (regionMaxOffset < endAddress)
						endAddress = regionMaxOffset;
				}

				for (uint64_t currentIndex = startingAddress; currentIndex < endAddress;) {

					for (uint64_t sigIndex = maxIndex; sigIndex >= 0; sigIndex--) {
						byte reading = *(byte*)((uint64_t)mbi.BaseAddress + currentIndex + sigIndex);

						if (reading != signature.at(sigIndex) && signature.at(sigIndex) != -1) {
							currentIndex += shiftTable[reading];
							break;
						}
						else if (sigIndex == 0) {

							if (signature.at(signature.size() - 1) != *(byte*)((uint64_t)mbi.BaseAddress + currentIndex + signature.size() - 1))
							{
								currentIndex += 1;
								break;
							}

							return (uint64_t)mbi.BaseAddress + currentIndex;

						}
					}
				}


				if (region != 0 && contador == region && !multipleRegions)
				{
					break;
				}

				i = (uint64_t)mbi.BaseAddress + mbi.RegionSize;
			}
		}
		return NULL;
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
	void ConsoleInit();
	void ConsoleDealloc();
	void LogPrint(const std::string_view& message_view);
	void LogPrint(const char* message);
	void LogPrint(const float messageFloat);
	std::string ReadLine();
	extern HANDLE debugConsoleHandle;
}

namespace Console {
	void ConsoleInit(std::string title);
	void ConsoleDealloc();
	void LogPrint(const std::string_view& message_view);
	void LogPrint(const char* message);
	void LogPrint(const float messageFloat);
	std::string ReadLine();
	extern HANDLE consoleHandle;
}