#pragma once

#include <string>

struct KeyCodeActor {
	KeyCodeActor(std::string name) {
		Name = name;
	}
	KeyCodeActor(std::string name, int num, bool randomized) {
		Name = name;
		Num = num;
		Randomized = randomized;
	}

	std::string Name;
	int Num = 1;
	bool Randomized = false;
};