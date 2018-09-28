#include <graphics/graphics.hpp>
#include <gui/gui.hpp>
#include <log/log.hpp>
#include <util/util.hpp>

GuiItemList::GuiItemList(const std::vector<std::wstring>& _strings)
        : strings(_strings)
        , listbox(nullptr)
        , selectedItemIndex(-1)
        , clickHandler([]() { return false; })
        , closeMenu(false)
{
    LOG("GuiItemList created");
    eventHandlerId = getEventReceiver().addEventHandler([this](const irr::SEvent& event) -> bool {
        // If this is not a GUI event, skip it
        if (event.EventType != irr::EET_GUI_EVENT) {
            return false;
        }
        // If it has nothing to do with out listbox, skip it too
        if (event.GUIEvent.Caller != listbox) {
            return false;
        }

        // LOG("Listbox event: " << wstring_cast(getGuiEventDebugName(event.GUIEvent.EventType)));
        if (event.GUIEvent.EventType == irr::gui::EGET_LISTBOX_CHANGED
            || event.GUIEvent.EventType == irr::gui::EGET_LISTBOX_SELECTED_AGAIN) {
            // Clicked
            LOG("Clicked");
            closeMenu = clickHandler();
            std::unique_lock<std::mutex> cv_lock(cv_mutex);
            cv_lock.unlock();
            cv.notify_one();
            cv_lock.lock();
        }
        return true;
    });
}

void GuiItemList::setClickHandler(const std::function<bool()>& callback)
{
    clickHandler = callback;
}

void GuiItemList::draw(const irr::core::recti& position)
{
    listbox = createListBox(strings, position);
    LOG("Acquiring unique lock (draw)");
    std::unique_lock<std::mutex> cv_lock(cv_mutex);
    LOG("Acquired unique lock (draw)");
    LOG("Waiting for CV (draw)");
    cv.wait(cv_lock, [this]() { return closeMenu; });
    LOG("Finished waiting for CV (draw)");
    LOG("Exiting (draw)");
}
std::wstring GuiItemList::getSelectedItem()
{
    return strings.at(getSelectedItemIndex());
}
size_t GuiItemList::getSelectedItemIndex()
{
    if (listbox == nullptr) {
        throw std::runtime_error("Attempted to get selected item before showing the item list");
    }
    if (listbox->getSelected() == -1) {
        throw std::runtime_error("Attempted to get selected item before user had selected an item");
    }
    return listbox->getSelected();
}

GuiItemList::~GuiItemList()
{
    listbox->remove();
    getEventReceiver().deleteEventHandler(eventHandlerId);
    LOG("GuiItemList removed");
}
