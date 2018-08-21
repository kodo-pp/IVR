#ifndef GUI_GUI_MANAGER_HPP
#define GUI_GUI_MANAGER_HPP

#include <unordered_set>

#include <gui/gui.hpp>

/**
 * Manages GUI elements
 *
 * WARNING: should be instantiated only once
 */
class GuiManager
{
public:
    GuiManager(std::unordered_set<GuiElement> _guiElements);
    virtual ~GuiManager();

    void drawAll();
    void addGuiElement(GuiElement elem);
    void removeGuiElement(GuiElement elem);

protected:
    std::unordered_set<GuiElement> guiElements;
};

// The only instance of this class
extern GuiManager guiManager;

#endif /* end of include guard: GUI_GUI_MANAGER_HPP */
