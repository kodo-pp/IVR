#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

#include <modbox/gui/gui.hpp>

class WorldSettings
{
public:
    std::optional<std::tuple<std::string, std::vector <std::string>, bool>> show();

private:
    irr::gui::IGUIButton* ok;
    irr::gui::IGUIButton* cancel;
    irr::gui::IGUIButton* all;
    irr::gui::IGUIButton* no;
    irr::gui::IGUIEditBox* title;
    irr::gui::IGUIEditBox* modules;
    irr::gui::IGUICheckBox* buildMode;
    std::condition_variable cv;
    bool doClose = false;
    mutable std::mutex cvMutex;
    std::optional<std::tuple<std::string, std::vector <std::string>, bool>> result;
};
