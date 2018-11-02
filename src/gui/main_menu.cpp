#include <unordered_map>

#include <modbox/game/game_loop.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/gui/main_menu.hpp>
#include <modbox/log/log.hpp>

template <typename K, typename V>
static std::vector<K> mapKeys(const std::unordered_map<K, V>& mp)
{
    std::vector<std::wstring> keys;
    keys.reserve(mp.size());
    for (const auto& [k, v] : mp) {
        keys.emplace_back(k);
    }
    return keys;
}

MainMenu::MainMenu(const std::unordered_map<std::wstring, std::function<void()>>& _actions)
        : itemList(mapKeys(_actions)), actions(_actions)
{
    itemList.setClickHandler([]() { return true; });
}

void MainMenu::show()
{
    while (true) {
        itemList.draw({0, 0, 400, 300});
        std::wstring selected = itemList.getSelectedItem();
        if (actions.count(selected) == 0) {
            throw std::logic_error("Inexistent main menu item selected");
        }
        setVisible(false);
        actions.at(selected)();
        setVisible(true);
    }
}

void MainMenu::setVisible(bool visible)
{
    std::lock_guard<std::recursive_mutex> lock(getIrrlichtMutex());
    LOG("MainMenu: setting visibility: " << visible);
    if (!visible) {
        itemList.hide();
    } else {
        itemList.show();
    }
    LOG("MainMenu: successfully set visibility: " << visible);
}
