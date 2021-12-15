#pragma once
#include <string>
#include <vector>
#include <map>

namespace EnemyData {
	struct Enemy {
		std::vector<std::string> Variants;
		std::vector<std::string> WieldableProfiles;
	};

	struct Weapon {

	};


	std::vector<std::string> Profiles = {"Sword", "LSword", "Spear", "Bow"};
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
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Bokoblin", enemy));
		}
		// Enemy_Moriblin
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear", "Bow" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Moriblin", enemy));
		}
		// Enemy_Lizalfos
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::vector<std::string> wieldableProfiles = { "Sword", "Lsword", "Spear", "Bow" };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Lizalfos", enemy));
		}
		// Enemy_Guardian
		{
			std::vector<std::string> variants = { "A", "B", "C" };
			std::vector<std::string> wieldableProfiles = {};
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian", enemy));
		}
		// -----------------------------------------------

		// WeaponClasses
		// -----------------------------------------------
		// Sword
		{
			Weapon weapon;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Sword", weapon));
		}
		// LSword
		{
			Weapon weapon;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("LSword", weapon));
		}
		// Spear
		{
			Weapon weapon;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Spear", weapon));
		}
		// Bow
		{
			Weapon weapon;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Bow", weapon));
		}
		// Shield
		{
			Weapon weapon;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Shield", weapon));
		}
	}
};