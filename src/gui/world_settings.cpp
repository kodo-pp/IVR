#include <modbox/gui/world_settings.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <boost/algorithm/string/split.hpp>

std::optional<std::tuple <std::string, std::vector <std::string>, bool>> WorldSettings::show()
{
    std::unique_lock<std::mutex> cvLock(cvMutex);
    irr::core::rectf titleRect(.1, .1, .9, .2);
    irr::core::rectf okRect(.1, .85, .45, .95);
    irr::core::rectf cancelRect(.55, .85, .9, .95);
    irr::core::rectf allRect(.1, .35, .45, .45);
    irr::core::rectf noRect(.55, .35, .9, .45);
    irr::core::rectf modulesRect(.1, .50, .9, .60);
    irr::core::rectf buildModeRect(.5, .75, .55, .80);
    title = getGuiEnvironment()->addEditBox(L"World name", graphicsViewportize(titleRect));
    ok = getGuiEnvironment()->addButton(graphicsViewportize(okRect), nullptr, 1, L"OK");
    cancel = getGuiEnvironment()->addButton(graphicsViewportize(cancelRect), nullptr, 2, L"Cancel");
    all = getGuiEnvironment()->addButton(graphicsViewportize(allRect), nullptr, 3, L"All");
    no = getGuiEnvironment()->addButton(graphicsViewportize(noRect), nullptr, 4, L"Clear");
    modules = getGuiEnvironment()->addEditBox(L"", graphicsViewportize(modulesRect));
    buildMode = getGuiEnvironment()->addCheckBox(false, graphicsViewportize(buildModeRect));

    auto eventHandlerId = getEventReceiver().addEventHandler([this](const irr::SEvent& event) -> bool {
        // If this is not a GUI event, skip it
        if (event.EventType != irr::EET_GUI_EVENT) {
            return false;
        }
        auto caller = event.GUIEvent.Caller;
        if (caller != ok && caller != no && caller != cancel && caller != all) {
            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
            if (caller == cancel) {
                result = {};
                doClose = true;
                std::unique_lock<std::mutex> cvLock(cvMutex);
                cvLock.unlock();
                cv.notify_one();
                cvLock.lock();
                return true;
            } else if (caller == ok) {
                std::vector <std::string> moduleList;
                std::string s = bytes_pack(modules->getText());
                boost::algorithm::split(moduleList, s, [](char c){return c == ' ';});
                result = {std::make_tuple(bytes_pack(title->getText()), moduleList, buildMode->isChecked())};
                doClose = true;
                std::unique_lock<std::mutex> cvLock(cvMutex);
                cvLock.unlock();
                cv.notify_one();
                cvLock.lock();
                return true;
            } else if (caller == all) {
                LOG("ALL");
            } else if (caller == no) {
                LOG("NO");
            }
        }
        return true;
    });

    cv.wait(cvLock, [this]() { return doClose; });
    usleep(100000);
    getEventReceiver().deleteEventHandler(eventHandlerId);
    ok->remove();
    cancel->remove();
    no->remove();
    all->remove();
    buildMode->remove();
    title->remove();
    modules->remove();
    return result;
}
