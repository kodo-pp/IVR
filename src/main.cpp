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
#include <modbox/core/virtual_module.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/gui/gui.hpp>
#include <modbox/gui/main_menu.hpp>
#include <modbox/log/log.hpp>
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
                                        setAimVisible(true);
                                        graphicsInitializeCollisions();
                                        gameLoop();
                                    }},

                                   {L"Load test virtual module",
                                    []() {
                                        auto x = new VirtualModule(
                                                "modules/test_virtual_module/libtestvmodule.so");
                                        x->spawnModuleThread();
                                    }},

                                   {L"Quit", []() { destroy(); }}});
                try {
                    mainMenu.show();
                } catch (const std::exception& e) {
                    LOG("AAA: " << wstring_cast(e.what()));
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
