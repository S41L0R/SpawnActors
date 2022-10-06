#include "Console.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using namespace ConsoleProcessor;

namespace Threads {
	/// <summary>
	/// Manages getting console input
	/// </summary>
	DWORD WINAPI ConsoleThread(LPVOID param) {
		while (true) {
			std::string line = Console::ReadLine();
			if (line.size() == 0)
				continue;

			std::vector<std::string> command;
			{
				std::istringstream ss(line);
				std::string word;

				while (ss >> word)
				{
					command.push_back(word);
				}
			}
			std::transform(command[0].begin(), command[0].end(), command[0].begin(), // Make the command string lowercase
				[](unsigned char c) { return std::tolower(c); });

			if (command[0] == "help") {
				Console::LogPrint(
					"Commands:\n"
					"'help' - Shows commands\n"
					"'keycode [key] [ [num] [actorname] ](s)' - Registers keycode for actor spawning\n"
					"'rmkeycode [key]' - Unregisters keycode for actor spawning\n"
					"'pos' - Print link's pos\n"
					"Note:\n"
					"Enemy names prefixed with: \\ will have their variants randomized.\n"
					"Enemy names prefixed with / will be spawned with random, compatible weapons."
				);
			}
			else if (command[0] == "keycode") {
				keycode_mutex->lock();
				if (command.size() >= 4) {
					char keycode = std::toupper(command[1][0]);

					std::vector<TriggeredActor> actVec;
					int actorCount = (command.size() - 2) / 2;

					for (int i = 0; i < command.size() - 2; i += 2) {
						int num = std::stoi(command[i + 2]);

						bool actorRandomized = false;
						bool weaponsRandomized = false;
						actorRandomized = (command[i + 3][0] == '\\') || (command[i + 3][1] == '\\');
						weaponsRandomized = (command[i + 3][0] == '/') || (command[i + 3][1] == '/');
						if (actorRandomized && weaponsRandomized)
							command[i + 3].erase(0, 2);
						else
							if (actorRandomized || weaponsRandomized)
								command[i + 3].erase(0, 1);

						actVec.push_back(TriggeredActor(command[i + 3], 0.f, 5.f, 0.f, true, num, actorRandomized, weaponsRandomized));
					}


					if (keyCodeMap->find(keycode) != keyCodeMap->end()) // Remove last version if it exists
						keyCodeMap->erase(keyCodeMap->find(keycode));

					keyCodeMap->insert({ keycode, actVec });
					prevKeyStateMap->insert({ keycode, false });
					Console::LogPrint("Keycode added succesfully");
				}
				else {
					Console::LogPrint("Format: keycode [key] [num] [actorname(s)]");
				}
				keycode_mutex->unlock();
			}
			else if (command[0] == "rmkeycode") {
				keycode_mutex->lock();
				if (command.size() == 2) {
					keyCodeMap->erase(std::toupper(command[1][0]));
					prevKeyStateMap->erase(std::toupper(command[1][0]));
					Console::LogPrint("Keycode Removed Succesfully");
				}
				else {
					Console::LogPrint("Format: rmkeycode [key]");
				}
				keycode_mutex->unlock();
			}
			else if (command[0] == "reloadconfig")
				registerPresetKeycodes();
			else if (command[0] == "pos" && isSetup) {
				Console::LogPrint(*memInstance->linkData.PosX);
				Console::LogPrint(*memInstance->linkData.PosY);
				Console::LogPrint(*memInstance->linkData.PosZ);
			}
		}
		return 0;
	}
}

namespace ConsoleProcessor {

	MemoryInstance* memInstance = nullptr;
	std::shared_mutex* keycode_mutex = nullptr;
	std::map<char, std::vector<TriggeredActor>>* keyCodeMap = nullptr;
	std::map<char, bool>* prevKeyStateMap = nullptr;
	bool* isSetup = nullptr;

	void registerPresetKeycodes() {
		std::ifstream txtFile;
		txtFile.open("keycodes.txt");

		std::string line;
		while (std::getline(txtFile, line)) {

			// Get our command vector
			std::vector<std::string> command;
			{
				std::istringstream ss(line);
				std::string word;

				while (ss >> word)
				{
					command.push_back(word);
				}
			}

			// Actually set the keycode!
			keycode_mutex->lock();
			if (command.size() >= 3) {
				char keycode = std::toupper(command[0][0]);


				std::vector<TriggeredActor> actVec;
				int actorCount = (command.size() - 1) / 2;

				for (int i = 0; i < command.size() - 1; i += 2) {
					int num = std::stoi(command[i + 1]);

					bool actorRandomized = false;
					bool weaponsRandomized = false;
					actorRandomized = (command[i + 2][0] == '\\') || (command[i + 2][1] == '\\');
					weaponsRandomized = (command[i + 2][0] == '/') || (command[i + 2][1] == '/');
					if (actorRandomized && weaponsRandomized)
						command[i + 2].erase(0, 2);
					else
						if (actorRandomized || weaponsRandomized)
							command[i + 2].erase(0, 1);

					actVec.push_back(TriggeredActor(command[i + 2], num, 0.f, 5.f, 0.f, true, actorRandomized, weaponsRandomized));
				}

				if (keyCodeMap->find(keycode) != keyCodeMap->end()) // Remove last version if it exists
					keyCodeMap->erase(keyCodeMap->find(keycode));

				keyCodeMap->insert({ keycode, actVec });
				prevKeyStateMap->insert({ keycode, false });
				Console::LogPrint("Keycode added succesfully");
			}
			keycode_mutex->unlock();
		}
	}
}