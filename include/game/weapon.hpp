#ifndef GAME_WEAPON_HPP
#define GAME_WEAPON_HPP

#include <mutex>
#include <string>
#include <unordered_map>

class WeaponManager
{
public:
    void addWeapon(const std::string& name, double length, double damage);
    void removeWeapon(const std::string& name);
    double getDamage(const std::string& name);
    double getLength(const std::string& name);
    bool hasWeapon(const std::string& name);

private:
    std::recursive_mutex mutex;
    std::unordered_map<std::string, std::pair<double, double>> weapons;
};

WeaponManager& getWeaponManager();

#endif /* end of include guard: GAME_WEAPON_HPP */
