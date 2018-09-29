#include <modbox/graphics/graphics.hpp>
#include <modbox/gui/gui.hpp>
#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>

GuiItemList::GuiItemList(const std::vector<std::wstring>& _strings)
        : strings(_strings)
        , listbox(nullptr)
        , selectedItemIndex(-1)
        , clickHandler([]() { return false; })
        , closeMenu(false)
        , isVisible(false)
{
    LOG("GuiItemList created");
}

void GuiItemList::setClickHandler(const std::function<bool()>& callback)
{
    clickHandler = callback;
}

void GuiItemList::draw(const irr::core::recti& position)
{
    this->position = position;
    show();
    std::unique_lock<std::mutex> cv_lock(cv_mutex);
    cv.wait(cv_lock, [this]() { return closeMenu; });
}

void GuiItemList::show()
{
    LOG("showing GuiItemList");
    if (isVisible) {
        return;
    }
    eventHandlerId = getEventReceiver().addEventHandler([this](const irr::SEvent& event) -> bool {
        // If this is not a GUI event, skip it
        if (event.EventType != irr::EET_GUI_EVENT) {
            return false;
        }
        // If it has nothing to do with out listbox, skip it too
        if (event.GUIEvent.Caller != listbox) {
            return false;
        }

        if (event.GUIEvent.EventType == irr::gui::EGET_LISTBOX_CHANGED
            || event.GUIEvent.EventType == irr::gui::EGET_LISTBOX_SELECTED_AGAIN) {
            // Clicked
            closeMenu = clickHandler();
            std::unique_lock<std::mutex> cv_lock(cv_mutex);
            cv_lock.unlock();
            cv.notify_one();
            cv_lock.lock();
        }
        return true;
    });
    LOG("EvH id = " << eventHandlerId);
    selectedItemIndex = -1;
    isVisible = true;
    closeMenu = false;
    listbox = createListBox(strings, position);
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
    if (isVisible) {
        getEventReceiver().deleteEventHandler(eventHandlerId);
        listbox->remove();
    }
    LOG("GuiItemList removed");
}

void GuiItemList::hide()
{
    if (!isVisible) {
        return;
    }
    isVisible = false;

    // hide
    getEventReceiver().deleteEventHandler(eventHandlerId);
    listbox->remove();
    listbox = nullptr;
    LOG("ItemList hidden");
}
