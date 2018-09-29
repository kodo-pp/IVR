#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include <future>
#include <map>
#include <string>
#include <vector>

#include <irrlicht.h>

/**
 * Represents an abstract GUI element.
 */
class GuiElement
{
public:
    GuiElement() = default;
    virtual ~GuiElement() = default;
    virtual void draw(const irr::core::recti& position) = 0;

protected:
    irr::core::recti position;
};

/**
 * List of string items
 *
 * One of items can be selected by user
 */
class GuiItemList : public GuiElement
{
public:
    GuiItemList(const std::vector<std::wstring>& _strings);
    virtual ~GuiItemList();

    void setClickHandler(const std::function<bool()>& callback);

    virtual void draw(const irr::core::recti& position) override;
    std::wstring getSelectedItem();
    size_t getSelectedItemIndex();

    virtual void hide();
    virtual void show();

protected:
    std::vector<std::wstring> strings;
    irr::gui::IGUIListBox* listbox;
    ssize_t selectedItemIndex;
    uint64_t eventHandlerId;
    std::function<bool()> clickHandler;
    std::condition_variable cv;
    std::mutex cv_mutex;
    bool closeMenu;
    bool isVisible;
    irr::core::recti position;
};

class GuiButton : public GuiElement
{
public:
    GuiButton(const std::wstring& _text);
    virtual void draw(const irr::core::recti& position) override;
    std::wstring getText();

protected:
    std::wstring text;
};

// WARNING: 'actions' in class diagram are 'buttons' here
class GuiDialog : public GuiElement
{
public:
    GuiDialog(std::wstring _text, std::vector<std::wstring> _buttons);

    // WARNING: unable to adhere to class diagram in this case because of the
    // limitations of C++: method in the derived class and overriden method
    // in the base class with the same names ('draw' in our case) and taking
    // the same arguments (no arguments in our case) must have the same return
    // types. They do not if and only if the method in the base class is not
    // declared as virtual, which is not true in our case.
    // So there will be a separate method to get the result of showing dialog
    // to user.
    virtual void draw(const irr::core::recti& position) override;

    int getResult(); // returns the index of the button pressed or -1 if the dialog
                     // was not shown to user yet

    std::wstring getText();
    void setText(std::wstring);

    std::vector<std::wstring>& getButtons(); // Returns reference, so this vector
                                             // CAN be modified
protected:
    std::vector<std::wstring> buttons;
};

#endif /* end of include guard: GUI_GUI_HPP */
