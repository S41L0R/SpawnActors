#pragma once
#include <string>
#include <vector>
#include <map>

namespace ActorData {

	struct Enemy {
		std::vector<std::string> Variants;
		std::map<std::string, int> WieldableProfiles;
		int MaxWeaponSlots = 0;
	};

	struct Weapon {
		std::vector<std::string> Variants;
		std::string profile;
	};

	std::map<std::string, Enemy> EnemyClasses;
	std::map<std::string, Weapon> WeaponClasses;


	void InitDefaultValues() {
		// EnemyClasses
		// -----------------------------------------------
		// Enemy_Bokoblin
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 2}, {"Spear", 2}, {"Bow", 2}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Bokoblin", enemy));
		}

		// Enemy_Moriblin
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 2}, {"Spear", 2}, {"Bow", 2}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Moriblin", enemy));
		}

		// Enemy_Lizalfos
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Gold" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 2}, {"Spear", 2}, {"Bow", 2}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Lizalfos", enemy));
		}

		// Enemy_Guardian
		{
			std::vector<std::string> variants = { "A", "B", "C" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 0}, {"Lsword", 0}, {"Spear", 0}, {"Bow", 0}, {"Shield", 0} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 0;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian", enemy));
		}

		// Enemy_Guardian_Mini_Baby
		{
			std::vector<std::string> variants = { "", "Dark" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 0}, {"Lsword", 0}, {"Spear", 0}, {"Bow", 0}, {"Shield", 0} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 0;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini_Baby", enemy));
		}

		// Enemy_Guardian_Mini_Junior
		{
			std::vector<std::string> variants = { "Dark", "Wipe", "" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 1}, {"Spear", 1}, {"Bow", 0}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 1;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini_Junior", enemy));
		}

		// Enemy_Guardian_Mini_Middle
		{
			std::vector<std::string> variants = { "Middle", "Middle_Dark", "Practice",};
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 1}, {"Spear", 1}, {"Bow", 0}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 2;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini", enemy));
		}

		// Enemy_Guardian_Mini_Senior
		{
			std::vector<std::string> variants = { "Dark", ""};
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 1}, {"Spear", 1}, {"Bow", 0}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 3;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Guardian_Mini_Senior", enemy));
		}

		// Enemy_Golem
		{
			std::vector<std::string> variants = { "Fire", "Ice", "Junior", "Middle", "Senior"};
			std::map<std::string, int> wieldableProfiles = { {"Sword", 0}, {"Lsword", 0}, {"Spear", 0}, {"Bow", 0}, {"Shield", 0} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			enemy.MaxWeaponSlots = 0;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Golem", enemy));
		}

		// Enemy_Giant
		{}

		// Enemy_SandWorm
		{}

		//Enemy_Lynel
		{
			std::vector<std::string> variants = { "", "Junior", "Dark", "Middle", "Senior", "Gold" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 1}, {"Lsword", 2}, {"Spear", 2}, {"Bow", 1}, {"Shield", 1} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Lynel", enemy));
		}

		//Enemy_Chuchu
		{
			std::vector<std::string> variants = { "Junior", "Middle", "Senior", "Fire_Junior", "Fire_Middle", "Fire_Senior", "Ice_Junior", "Ice_Middle", "Ice_Senior", "Electric_Junior", "Electric_Middle", "Electric_Senior" };
			std::map<std::string, int> wieldableProfiles = { {"Sword", 0}, {"Lsword", 0}, {"Spear", 0}, {"Bow", 0}, {"Shield", 0} };
			Enemy enemy;
			enemy.Variants = variants;
			enemy.WieldableProfiles = wieldableProfiles;
			EnemyClasses.insert(std::pair<const std::string, Enemy>("Enemy_Chuchu", enemy));
		}

		// -----------------------------------------------

		// WeaponClasses
		// -----------------------------------------------
		// Sword
		{
			std::vector<std::string> variants = {};
			for (int i = 1; i <= 73; i++) {
				std::ostringstream ostr;
				ostr << std::setfill('0') << std::setw(3) << i;
				variants.push_back(ostr.str());
			}

			Weapon weapon;
			weapon.profile = (std::string)"Sword";
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Sword", weapon));
		}

		// Lsword
		{
			std::vector<std::string> variants = {};
			for (int i = 1; i <= 60; i++) {
				std::ostringstream ostr;
				ostr << std::setfill('0') << std::setw(3) << i;
				variants.push_back(ostr.str());
			}

			Weapon weapon;
			weapon.profile = (std::string)"Lsword";
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Lsword", weapon));
		}
		// Spear
		{
			std::vector<std::string> variants = {};
			for (int i = 1; i <= 50; i++) {
				std::ostringstream ostr;
				ostr << std::setfill('0') << std::setw(3) << i;
				variants.push_back(ostr.str());
			}

			Weapon weapon;
			weapon.profile = (std::string)"Spear";
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Spear", weapon));
		}
		// Bow
		{
			std::vector<std::string> variants = {};
			for (int i = 1; i <= 40; i++) {
				std::ostringstream ostr;
				ostr << std::setfill('0') << std::setw(3) << i;
				variants.push_back(ostr.str());
			}

			Weapon weapon;
			weapon.profile = (std::string)"Bow";
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Bow", weapon));
		}
		// Shield
		{
			std::vector<std::string> variants = {};
			for (int i = 1; i <= 42; i++) {
				std::ostringstream ostr;
				ostr << std::setfill('0') << std::setw(3) << i;
				variants.push_back(ostr.str());
			}

			Weapon weapon;
			weapon.profile = (std::string)"Shield";
			weapon.Variants = variants;
			WeaponClasses.insert(std::pair<const std::string, Weapon>("Weapon_Shield", weapon));
		}
	}
};