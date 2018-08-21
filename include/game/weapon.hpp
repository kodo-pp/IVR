#ifndef GAME_WEAPON_HPP
#define GAME_WEAPON_HPP

// XXX: maybe add class Tool and derive Weapon from it

/**
 * Represents an abstract weapon
 */

class Weapon
{
public:
    // XXX: maybe c-tor should exist
    Weapon() = delete;
    virtual ~Weapon();

    virtual void use() = 0;

    double getDurabilityLeft();
    void setDurabilityLeft(double durability);

    double getDurabilityMax();
    void setDurabilityMax(double durability);

    double getBaseDamage();
    // NOTICE: no setter

protected:
    double healthLeft;
    double healthMax;
    double baseDamage;
};

#endif /* end of include guard: GAME_WEAPON_HPP */
