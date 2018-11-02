#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <modbox/core/core.hpp>
#include <modbox/core/destroy.hpp>
#include <modbox/core/event_manager.hpp>
#include <modbox/core/init.hpp>
#include <modbox/core/memory_manager.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/gui/gui.hpp>
#include <modbox/gui/main_menu.hpp>
#include <modbox/gui/world_settings.hpp>
#include <modbox/log/log.hpp>
#include <modbox/modules/module_manager.hpp>
#include <modbox/net/net.hpp>
#include <modbox/util/handle_storage.hpp>
#include <modbox/util/util.hpp>
#include <modbox/world/world.hpp>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

struct CloseMenu {};

int main(int argc, char** argv)
{
    try {
        std::vector<std::string> args(argv, argv + argc);
        init(args);

        LOG("ModBox version " << _PROJECT_VERSION);
        createModuleListenerThread();
        LOG("Creating game thread");

        auto createWorldFunc = []() {
            WorldSettings ws;
            auto maybeResult = ws.show();
            if (!maybeResult.has_value()) {
                return;
            } else {
                try {
                    auto [name, modules, buildMode] = *maybeResult;
                    LOG("Creating world '" << name << "'");
                    createWorld(name, modules, buildMode);
                } catch (const std::runtime_error& e) {
                    LOG("Error while creating world: " << e.what());
                    return;
                }
            }
        };
        auto loadWorldFunc = []() {
            // Not implemented
            LOG("No u");
            assert("not" == "implemented");
        };
        /*
        auto enterWorldFunc = []() {
            moduleManager.loadModule("aim");
            moduleManager.loadModule("inventory");
            moduleManager.loadModule("test_enemy");
            moduleManager.loadModule("test_game_object");
            graphicsInitializeCollisions();
            gameLoop();
        };
        */


        auto singleplayerMenuFunc = [&]() {
                MainMenu singleplayerMenu({
                    {L"Create new world", createWorldFunc},
                    {L"Load existing world", loadWorldFunc},
                    {L"Back", []() {throw CloseMenu();}}
                });
                try {
                    singleplayerMenu.show();
                } catch (const CloseMenu& close) {
                    // do nothing
                    singleplayerMenu.setVisible(false);
                }
        };

        std::thread gameThread([&]() {
            try {
                MainMenu mainMenu({
                    {L"Singleplayer", singleplayerMenuFunc},
                    {L"Quit", []() { destroy(); }}
                });
                try {
                    mainMenu.show();
                } catch (const std::exception& e) {
                    LOG("Exception caught while showing main menu: " << wstring_cast(e.what()));
                }
            } catch (const std::exception& e) {
                LOG("Exception caught at game thread lambda: " << wstring_cast(e.what()));
            }
        });
        drawLoop();
        gameThread.join();
        destroy();
    } catch (const std::logic_error& e) {
        LOG("Logic error caught in main(): " << wstring_cast(e.what()));
        std::terminate();
    } catch (const std::exception& e) {
        LOG("Exception caught in main(): " << wstring_cast(e.what()));
        std::terminate();
    } catch (...) {
        LOG("Unknown exception caught in main()");
        std::terminate();
    }
}
