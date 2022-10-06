#pragma once

#include <string>

struct TriggeredActor {
	TriggeredActor(std::string name) {
		Name = name;
	}
	TriggeredActor(std::string name, int num, float xOffset, float yOffset, float zOffset, bool addPlayerPosOffset, bool actorRandomized, bool weaponsRandomized) {
		Name = name;
		Num = num;

		XOffset = xOffset;
		YOffset = yOffset;
		ZOffset = zOffset;
		AddPlayerPosOffset = addPlayerPosOffset;

		ActorRandomized = actorRandomized;
		WeaponsRandomized = weaponsRandomized;
	}

	std::string Name;
	int Num = 1;

	float XOffset; // Maybe if/when I rewrite this I'll do things neater.
	float YOffset; // But for now I've given up.
	float ZOffset; // These are either offsets from 0 0 0, or, if AddPlayerPos, from the player position.
	bool AddPlayerPosOffset;

	bool ActorRandomized = false;
	bool WeaponsRandomized = false;
};