#ifndef GAME_INVENTORY_ITEM_HPP
#define GAME_INVENTORY_ITEM_HPP

#include <game/game_object.hpp>

class InventoryItem : public GameObject
{
public:
    InventoryItem();

    double getWeight();

protected:
    double weight;
};

#endif /* end of include guard: GAME_INVENTORY_ITEM_HPP */
