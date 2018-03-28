#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include <string>
#include <vector>
#include <geometry/geometry.hpp>

/**
 * Represents an abstract GUI element.
 *
 * SHOULD NOT BE INSTANTIATED
 */
class GuiElement {
public:
    GuiElement() = delete;
    virtual ~GuiElement();
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

// WARNING: 'actions' in class diagram are 'buttons' here
class GuiDialog : public GuiElement {
public:
    GuiDialog(std::wstring _text, std::vector <std::wstring> _buttons);

    // WARNING: unable to adhere to class diagram in this case because of the
    // limitations of C++: method in the derived class and overriden method
    // in the base class with the same names ('draw' in our case) and taking
    // the same arguments (no arguments in our case) must have the same return
    // types. They do not if and only if the method in the base class is not
    // declared as virtual, which is not true in our case.
    // So there will be a separate method to get the result of showing dialog
    // to user.
    void draw() override;

    int getResult(); // returns the index of the button pressed or -1 if the dialog
                     // was not shown to user yet

    std::wstring getText();
    void setText(std::wstring);

    std::vector <std::wstring> & getButtons(); // Returns reference, so this vector
                                               // CAN be modified
protected:
    std::vector <std::wstring> buttons;
};

#endif /* end of include guard: GUI_GUI_HPP */
