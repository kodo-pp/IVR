#include <unordered_map>

#include <game/game_loop.hpp>
#include <graphics/graphics.hpp>
#include <gui/main_menu.hpp>
#include <log/log.hpp>

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
    setAimVisible(false);
    itemList.draw({0, 0, 400, 300});
    std::wstring selected = itemList.getSelectedItem();
    if (actions.count(selected) == 0) {
        throw std::logic_error("Inexistent main menu item selected");
    }
    setVisible(false);
    actions.at(selected)();
    setVisible(true);
}

void MainMenu::setVisible(bool visible)
{
    if (visible) {
        setAimVisible(false);
    }
    std::lock_guard<std::recursive_mutex> lock(getIrrlichtMutex());
    itemList.setVisible(visible);
}
