#pragma once
#include <string>
#include <vector>
#include <map>

namespace ActorData {
	struct Enemy {
		std::vector<std::string> Variants;
		std::vector<std::string> WieldableProfiles;
		int MaxWeaponSlots = 0;
	};

	struct Weapon {
		std::vector<std::string> Variants;
	};


	std::vector<std::string> Profiles = {"Sword", "Lsword", "Spear", "Bow"};
	std::map<std::string, Enemy> EnemyClasses;
	std::map<std::string, Weapon> WeaponClasses;


	void InitDefaultValues() {
		// EnemyClasses
		// -----------------------------------------------
		// Enemy_Bokoblin
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear", "Bow" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Bokoblin", enemy));
		}

		// Enemy_Moriblin
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear", "Bow" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Moriblin", enemy));
		}

		// Enemy_Lizalfos
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear", "Bow" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Lizalfos", enemy));
		}

		// Enemy_Guardian
		{
			std::vector<std::string> variants = { "A", "B", "C" };
			std::vector<std::string> wieldableProfiles = {};
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 0;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian", enemy));
		}

		// Enemy_Guardian_Mini_Baby
		{
			std::vector<std::string> variants = { "", "Dark" };
			std::vector<std::string> wieldableProfiles = {};
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 0;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini_Baby", enemy));
		}

		// Enemy_Guardian_Mini_Junior
		{
			std::vector<std::string> variants = { "Dark", "Wipe", "" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			//enemy.MaxWeaponSlots = 1;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini_Junior", enemy));
		}

		// Enemy_Guardian_Mini_Middle
		{
			std::vector<std::string> variants = { "Middle", "Middle_Dark", "Practice",};
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			//enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini", enemy));
		}

		// Enemy_Guardian_Mini_Senior
		{
			std::vector<std::string> variants = { "Dark", ""};
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			//enemy.MaxWeaponSlots = 3;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini_Senior", enemy));
		}

		// Enemy_Golem
		{
			std::vector<std::string> variants = { "Fire", "Ice", "Junior", "Middle", "Senior"};
			std::vector<std::string> wieldableProfiles = {};
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Golem", enemy))
		}

		// Enemy_Giant
		{}

		// Enemy_SandWorm
		{}

		//Enemy_Lynel
		{
			std::vector<std::string> variants = { "", "Junior", "Dark", "Middle", "Senior", "Gold" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear", "Bow" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Lynel", enemy))
		}

		// -----------------------------------------------

		// WeaponClasses
		// -----------------------------------------------
		// Sword
		{
			std::vector<std::string> variants = { "073", "023" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Sword", weapon));
		}

		// Shield - This will need to be dynamically linked and randomized if the chosen enemy uses a sword
		{
			std::vector<std::string> variants = {"038", "030"};
			Weapon weapon;
			weapon.Variants = std::vector<std::string> Variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Shield", weapon))
		}

		// Lsword
		{
			std::vector<std::string> variants = { "097", "055" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Lsword", weapon));
		}
		// Spear
		{
			std::vector<std::string> variants = { "038", "024" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Spear", weapon));
		}
		// Bow
		{
			std::vector<std::string> variants = { "032", "016" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Bow", weapon));
		}
		// Shield
		{
			std::vector<std::string> variants = { "014", "018" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Shield", weapon));
		}
	}
};