#ifndef GAME_INVENTORY_ITEM_HPP
#define GAME_INVENTORY_ITEM_HPP

class InventoryItem : public GameObject {
public:
    SolidObjectPart() = delete;

    double getWeight();

protected:
    double weight;
};

#endif /* end of include guard: GAME_INVENTORY_ITEM_HPP */
