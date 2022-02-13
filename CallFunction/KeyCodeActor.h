#pragma once

#include <string>

struct KeyCodeActor {
	KeyCodeActor(std::string name) {
		Name = name;
	}
	KeyCodeActor(std::string name, int num, bool actorRandomized, bool weaponsRandomized) {
		Name = name;
		Num = num;
		ActorRandomized = actorRandomized;
		WeaponsRandomized = weaponsRandomized;
	}

	std::string Name;
	int Num = 1;
	bool ActorRandomized = false;
	bool WeaponsRandomized = false;
};