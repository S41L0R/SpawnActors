#pragma once
#include <string>
#include <vector>
#include <map>

namespace ActorData {
	struct Enemy {
		std::vector<std::string> Variants;
		std::vector<std::string> WieldableProfiles;
	};

	struct Weapon {
		std::vector<std::string> Variants;
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
			std::vector<std::string> variants = { "073", "023" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Sword", weapon));
		}
		// LSword
		{
			std::vector<std::string> variants = { "097", "055" };
			Weapon weapon;
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_LSword", weapon));
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