#include <graphics/graphics.hpp>
#include <gui/gui.hpp>
#include <log/log.hpp>

GuiItemList::GuiItemList(const std::vector<std::wstring>& _strings)
        : strings(_strings), listbox(nullptr), selectedItemIndex(-1)
{
}

void GuiItemList::draw(const irr::core::recti& position)
{
    listbox = createListBox(strings, position);
    // wait until user selects something
    // stub
    sleep(5);
}
std::wstring GuiItemList::getSelectedItem()
{
    // Should return an index
    // stub
    if (listbox != nullptr) {
        return L"Test. Test. Test.";
    } else {
        throw std::runtime_error("Attempted to get selected item before showing the item list");
    }
}
size_t GuiItemList::getSelectedItemIndex()
{
    // Should return an index
    // stub
    if (listbox != nullptr) {
        return 0;
    } else {
        throw std::runtime_error(
                "Attempted to get selected item index before showing the item list");
    }
}

GuiItemList::~GuiItemList()
{
    listbox->remove();
    LOG("Removed");
}
