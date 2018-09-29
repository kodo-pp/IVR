#ifndef GUI_MAIN_MENU_HPP
#define GUI_MAIN_MENU_HPP

#include <functional>
#include <string>
#include <unordered_map>

#include <modbox/gui/gui.hpp>

class MainMenu
{
public:
    explicit MainMenu(const std::unordered_map<std::wstring, std::function<void()>>& _actions);
    MainMenu(const MainMenu& other) = default;
    MainMenu(MainMenu&& other) = default;
    virtual ~MainMenu() = default;

    MainMenu& operator=(const MainMenu& other) = default;
    MainMenu& operator=(MainMenu&& other) = default;

    virtual void show();
    virtual void setVisible(bool visible);

private:
    GuiItemList itemList;
    std::unordered_map<std::wstring, std::function<void()>> actions;
};

#endif /* end of include guard: GUI_MAIN_MENU_HPP */
