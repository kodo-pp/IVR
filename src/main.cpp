#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <modbox/core/core.hpp>
#include <modbox/core/destroy.hpp>
#include <modbox/core/init.hpp>
#include <modbox/core/memory_manager.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/gui/gui.hpp>
#include <modbox/gui/main_menu.hpp>
#include <modbox/log/log.hpp>
#include <modbox/modules/module_manager.hpp>
#include <modbox/net/net.hpp>
#include <modbox/util/handle_storage.hpp>
#include <modbox/util/util.hpp>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    try {
        std::vector<std::string> args(argv, argv + argc);
        init(args);

        LOG(L"ModBox version " << _PROJECT_VERSION);
        createModuleListenerThread();
        LOG("Creating game thread");
        std::thread gameThread([]() {
            try {
                MainMenu mainMenu({{L"Singleplayer",
                                    []() {
                                        try {
                                            // moduleManager.loadModule("test");
                                            moduleManager.loadModule("aim");
                                        } catch (const std::exception& e) {
                                            LOG("Failed to load module: " << e.what());
                                            return;
                                        }
                                        // setAimVisible(true);
                                        graphicsInitializeCollisions();
                                        gameLoop();
                                    }},

                                   // {L"Load test module",
                                   //  []() {
                                   //      try {
                                   //          moduleManager.loadModule("test");
                                   //      } catch (const std::exception& e) {
                                   //          LOG("Failed to load module: " << e.what());
                                   //      }
                                   //  }},

                                   {L"Quit", []() { destroy(); }}});
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
