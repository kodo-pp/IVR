#include <modbox/game/weapon.hpp>
#include <stdexcept>
#include <exception>

void WeaponManager::addWeapon(const std::string& name, double length, double damage)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (weapons.count(name) > 0) {
        throw std::runtime_error("Weapon '" + name + "' already exists");
    }
    weapons.emplace(name, std::make_pair(length, damage));
}
void WeaponManager::removeWeapon(const std::string& name)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (weapons.count(name) == 0) {
        throw std::runtime_error("Weapon '" + name + "' does not exist");
    }
    weapons.erase(name);
}
double WeaponManager::getDamage(const std::string& name)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (weapons.count(name) == 0) {
        throw std::runtime_error("Weapon '" + name + "' does not exist");
    }
    return weapons.at(name).second;
}
double WeaponManager::getLength(const std::string& name)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (weapons.count(name) == 0) {
        throw std::runtime_error("Weapon '" + name + "' does not exist");
    }
    return weapons.at(name).first;
}
bool WeaponManager::hasWeapon(const std::string& name)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    return weapons.count(name) > 0;
}

WeaponManager& getWeaponManager()
{
    static WeaponManager manager;
    return manager;
}
