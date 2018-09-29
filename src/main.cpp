#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <core/core.hpp>
#include <core/destroy.hpp>
#include <core/init.hpp>
#include <core/memory_manager.hpp>
#include <game/game_loop.hpp>
#include <graphics/graphics.hpp>
#include <gui/gui.hpp>
#include <gui/main_menu.hpp>
#include <log/log.hpp>
#include <net/net.hpp>
#include <util/handle_storage.hpp>
#include <util/util.hpp>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

struct FuncResult* testFunc(const std::vector<void*>& args)
{
    assert(!args.empty());
    return (struct FuncResult*)args[0];
}

int main(int argc, char** argv)
{
    try {
        std::vector<std::string> args(argv, argv + argc);
        init(args);

        LOG(L"ModBox version " << _PROJECT_VERSION);
        createModuleListenerThread();
        LOG("Creating draw thread");
        std::thread gameThread([]() {
            try {
                MainMenu mainMenu({{L"Singleplayer",
                                    []() {
                                        setAimVisible(true);
                                        graphicsInitializeCollisions();
                                        gameLoop();
                                    }},
                                   {L"Quit", []() { destroy(); }}});
                mainMenu.show();
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
