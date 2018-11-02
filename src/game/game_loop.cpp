#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <modbox/core/core.hpp>
#include <modbox/core/dyntype.hpp>
#include <modbox/core/init.hpp>
#include <modbox/game/enemy.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/game/player.hpp>
#include <modbox/game/solid_object.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/log/log.hpp>
#include <modbox/util/util.hpp>
#include <modbox/world/terrain.hpp>
#include <modbox/core/event_manager.hpp>
#include <modbox/game/weapon.hpp>

#include <irrlicht_wrapper.hpp>
#include <unistd.h>

std::recursive_mutex irrlichtMutex;

std::atomic<bool> canPlaceObject(true);
std::atomic<bool> gameStarted(false);
std::atomic<bool> safeDrawFunctionsRun(false); // Костыль, но работает (теперь нет)

std::vector<std::pair<std::string, uint64_t>> eachTickFuncs;

std::recursive_mutex drawFunctionsMutex;
std::vector<std::packaged_task<void()>> drawFunctions;

static const int desiredFps = 60;

static std::optional<std::thread::id> drawThreadId;
std::thread::id getDrawThreadId()
{
    if (!drawThreadId.has_value()) {
        throw std::logic_error("Attempted to get draw thread ID before it was set");
    }
    return drawThreadId.value();
}

Player& getPlayer()
{
    static Player player(graphicsGetCamera(), graphicsGetPseudoCamera());
    return player;
}

std::recursive_mutex& getDrawFunctionsMutex()
{
    return drawFunctionsMutex;
}
std::recursive_mutex& getIrrlichtMutex()
{
    return irrlichtMutex;
}

static void processKeys(Player& player)
{
    // XXX: This is stub, camera movement and rotation should be done by class like Player
    IrrEventReceiver& receiver = getEventReceiver();

    // Camera movement (horizontal)
    {
        int dx = 0, dz = 0;
        if (receiver.isKeyPressed(irr::KEY_KEY_W)) {
            ++dz;
        }
        if (receiver.isKeyPressed(irr::KEY_KEY_D)) {
            ++dx;
        }
        if (receiver.isKeyPressed(irr::KEY_KEY_A)) {
            --dx;
        }
        if (receiver.isKeyPressed(irr::KEY_KEY_S)) {
            --dz;
        }

        double directionOffset = 0;
        // TODO: Убрать зависимость от fps
        double speed = 10.0 / desiredFps * 30;
        switch (dx * 10 + dz) {
        case -10 + -1: // back, left
            directionOffset = -0.75 * M_PI;
            break;
        case -10 + 0: // left
            directionOffset = -0.5 * M_PI;
            break;
        case -10 + 1: // forward, left
            directionOffset = -0.25 * M_PI;
            break;
        case 0 + -1: // back
            directionOffset = M_PI;
            break;
        case 0 + 0: // no movement
            directionOffset = 0;
            speed = 0;
            break;
        case 0 + 1: // forward
            directionOffset = 0;
            break;
        case 10 + -1: // back, right
            directionOffset = 0.75 * M_PI;
            break;
        case 10 + 0: // right
            directionOffset = 0.5 * M_PI;
            break;
        case 10 + 1: // forward, right
            directionOffset = 0.25 * M_PI;
            break;
        }

        player.moveForward(speed, directionOffset);
    }

    // Camera movement (vertical)
    {
        if (receiver.isKeyPressed(irr::KEY_SPACE)) {
            // Не спрашивайте, как я до этого дошёл и почему это должно работать
            // Но оно работает, и высота прыжка почти не зависит от FPS
            const double jumpHeight = 10.0 / pow(desiredFps, 0.33) * pow(30, 0.33);
            player.jump(jumpHeight);
        }
    }

    // Camera rotation
    {
        double dx = 0, dy = 0;
        if (receiver.isKeyPressed(irr::KEY_UP)) {
            dx -= 1.0;
        }
        if (receiver.isKeyPressed(irr::KEY_RIGHT)) {
            dy += 1.0;
        }
        if (receiver.isKeyPressed(irr::KEY_DOWN)) {
            dx += 1.0;
        }
        if (receiver.isKeyPressed(irr::KEY_LEFT)) {
            dy -= 1.0;
        }

        auto mouseDelta = receiver.getMouseDelta();
        const double mouseSensivity = 0.2;
        dx += mouseDelta.X * mouseSensivity;
        dy += mouseDelta.Y * mouseSensivity;

        const double speed = 2.0 / desiredFps * 30;
        player.turn(speed * dx, speed * dy);
    }
}

double getPlaceLength()
{
    return 500.0;
}

void gameLoop()
{
    gameStarted = true;
    Player& player = getPlayer();
    drawBarrier();

    getWeaponManager().addWeapon("sword", 500.0, 1.0);
    getWeaponManager().addWeapon("rifle", 10000.0, 5.0);

    addSelectorKind("enemies");

    getFuncProvider("inventory.addItems")({"literally nothing", "10"});
    getFuncProvider("inventory.addItems")({"sword", "1"});
    getFuncProvider("inventory.addItems")({"rifle", "1"});
    getFuncProvider("inventory.addItems")({"a bit of magic", "1"});

    getEventManager().addEventHandler("mouse.leftButtonDown", [](const std::unordered_map<std::string, std::string>&) {
        auto currentItem = getFuncProvider("inventory.getCurrentCellItem")({}).data.at(0);
        getEventManager().raiseEvent("use.l", {{"item", currentItem}});
    });

    getEventManager().addEventHandler("mouse.middleButtonDown", [](const std::unordered_map<std::string, std::string>&) {
        getFuncProvider("inventory.shiftCurrentCell")({"1"});
        LOG("Shifting inventory");
    });
    getGameObjectManager().addRecipe("testGameObject", "a bit of magic", "anotherGameObject");
    getEventManager().addEventHandler("gameObject.partAttach", [](const std::unordered_map<std::string, std::string>& args) {
        LOG("Attached '" << args.at("partKind") << "' to '" << args.at("kind") << "': got '" << args.at("resultingKind") << "'");
    });

    getEventManager().addEventHandler("use.l", [](const std::unordered_map<std::string, std::string>& args) {
        if (getWeaponManager().hasWeapon(args.at("item"))) {
            // Attack enemy
            auto length = getWeaponManager().getLength(args.at("item"));
            auto damage = getWeaponManager().getDamage(args.at("item"));
            auto maybeValue = getRayIntersect(getPlayer().getPosition().toIrrVector3df(), getCameraTarget(length), "enemies");
            if (maybeValue.has_value()) {
                auto& [point, drawable] = *maybeValue;
                if (auto maybeEnemyId = enemyManager.reverseLookup(drawable); maybeEnemyId.has_value()) {
                    enemyManager.mutableAccessEnemy(*maybeEnemyId).hit(damage);
                }
            }
            return;
        }
        // Attach part to object
        auto maybeValue = getRayIntersect(getPlayer().getPosition().toIrrVector3df(), getCameraTarget(getPlaceLength()), "gameObjects");
        if (maybeValue.has_value()) {
            auto& [point, drawable] = *maybeValue;
            if (auto maybeGameObjectId = getGameObjectManager().reverseLookup(drawable); maybeGameObjectId.has_value()) {
                getGameObjectManager().mutableAccess(*maybeGameObjectId).attachPart(args.at("item"));
            }
        }
    });

    int counter = 0;
    double i = 0;
    while (true) /* irrDeviceRun() can cause segfault */ {
        size_t idx = 0;
        std::vector<size_t> toRemove;
        ++counter;
        if (counter == desiredFps / 10) {
            counter = 0;
            terrainManager.autoLoad(player.getPosition().x, player.getPosition().z);
            for (const auto& fp : eachTickFuncs) {
                try {
                    auto arg = DyntypeCaster<std::string>::get(fp.second);
                    auto ret = getFuncProvider(fp.first)({arg});
                    if (ret.data.size() != 0) {
                        LOG("ret.data.size() != 0");
                    }
                } catch (const std::exception& e) {
                    LOG("Exception caught at game_loop(): " << wstring_cast(e.what()));
                    logStackTrace();
                    LOG("This funcProvider will be removed from each-tick execution list");
                    toRemove.push_back(idx);
                    break;
                }
                ++idx;
            }
        }

        for (const auto& i : toRemove) {
            LOG("Removing entry " << i << " from each-tick execution list");
            eachTickFuncs.erase(eachTickFuncs.begin() + i);
        }
        toRemove.clear();

        std::ignore = graphicsGetPlacePosition(player.getPosition(), player.getCameraTarget());
        double timeForFrame = 1.0 / desiredFps;
        i += timeForFrame;
        usleep(1000000 / 60);
    }
    destroy();
}

void eachTickWithParam(const std::string& name, uint64_t param)
{
    eachTickFuncs.emplace_back(name, param);
}

void drawLoop()
{
    drawThreadId = std::this_thread::get_id();
    int fpsCounter = 0;
    double oneSecondCounter = 0.0;

    double timeForFrame = 1.0 / desiredFps;

    safeDrawFunctionsRun = true;
    while (irrDeviceRun()) {
        if (doWeNeedToShutDown) {
            break;
        }

        {
            std::lock_guard<std::recursive_mutex> lock(drawFunctionsMutex);
            for (auto& func : drawFunctions) {
                try {
                    func();
                } catch (const std::exception& e) {
                    LOG("draw function: exception: " << wstring_cast(e.what()));
                    throw e;
                }
            }
            drawFunctions.clear();
        }

        auto timeBefore = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::recursive_mutex> lock(irrlichtMutex);
            graphicsDraw();
            if (gameStarted) {
                processKeys(getPlayer());
                try {
                    enemyManager.processAi();
                } catch (const std::exception& e) {
                    LOG("Exception caught at enemyManager.processAi(): " << e.what());
                }
                // getPlayer().moveForward(0, 0);
                // getPlayer().turn(0, 0);
            }
        }
        ++fpsCounter;

        auto timeAfter = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(timeAfter
                                                                                  - timeBefore);
        double timeToSleep = timeForFrame - duration.count();
        if (oneSecondCounter > 1.0) {
            fpsCounter = 0;
            oneSecondCounter = 0.0;
        }
        if (timeToSleep < 0.0) {
            // Comment it out to prevent spamming about low FPS. TODO: make it a config option
            LOG("Warning: frame rendering took longer than 1 / " << desiredFps << " s");
            LOG("Time to sleep is " << timeToSleep);
        } else {
            usleep(static_cast<int>(timeToSleep * 1e+6));
        }

        auto timeAfterSleep = std::chrono::high_resolution_clock::now();
        auto fullDuration = std::chrono::duration_cast<std::chrono::duration<double>>(timeAfterSleep
                                                                                      - timeBefore);
        oneSecondCounter += fullDuration.count();
    }
    destroy();
}
