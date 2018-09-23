#ifndef GUI_MAIN_MENU_HPP
#define GUI_MAIN_MENU_HPP

#include <functional>
#include <string>
#include <unordered_map>

class MainMenu
{
public:
    explicit MainMenu(const std::unordered_map<std::string, std::function<void(void)>>& actions);
    MainMenu(const MainMenu& other) = default;
    MainMenu(MainMenu&& other) = default;
    virtual ~MainMenu() = default;

    virtual void show();

    MainMenu& operator=(const MainMenu& other) = default;
    MainMenu& operator=(MainMenu&& other) = default;
};

#endif /* end of include guard: GUI_MAIN_MENU_HPP */
