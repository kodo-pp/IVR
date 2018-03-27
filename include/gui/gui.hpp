#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include <geometry/geometry.hpp>
#include <string>

/**
 * Represents an abstract GUI element.
 *
 * SHOULD NOT BE INSTANTIATED
 */
class GuiElement {
public:
    GuiElement() = delete;
    virtual void draw() = 0;
protected:
    Rectangle <double> position;
};

/**
 * List of string items
 *
 * One of items can be selected by user
 */
class GuiItemList : public GuiElement {
public:
    void draw() override;
    std::wstring getSelectedItem();
    size_t getSelectedItemIndex();
protected:
    size_t selectedItemIndex;
};

class GuiButton : public GuiElement {
public:
    void draw() override;
    std::wstring getText();
};

#endif /* end of include guard: GUI_GUI_HPP */
