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

#include <irrlicht_wrapper.hpp>
#include <unistd.h>

std::recursive_mutex irrlichtMutex;

std::atomic<bool> canPlaceObject(true);
std::atomic<bool> gameStarted(false);
std::atomic<bool> safeDrawFunctionsRun(false); // Костыль, но работает (теперь нет)

// TODO: use TerrainManager or something like that
std::vector<GameObjCube> placedCubes;
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

    // Place object
    {
        if (canPlaceObject && receiver.isKeyPressed(irr::KEY_KEY_C)) {
            bool hasHit;
            GamePosition hitPoint;
            std::tie(hasHit, hitPoint) = graphicsGetPlacePosition(player.getPosition(),
                                                                  player.getCameraTarget());

            if (hasHit) {
                LOG("Object placed at " << hitPoint);
                placedCubes.push_back(graphicsCreateCube());
                placedCubes.back().setPosition(hitPoint);
                placedCubes.back().sceneNode()->setScale({10, 10, 10});
                graphicsAddTexture(placedCubes.back(), graphicsLoadTexture("textures/cube3.png"));
                graphicsHandleCollisionsBoundingBox(placedCubes.back().sceneNode());
                canPlaceObject = false;
                delayedAssign(canPlaceObject, 0.4, true);
            }
        }
    }
}

void gameLoop()
{
    gameStarted = true;
    Player& player = getPlayer();
    drawBarrier();

    try {
        usleep(100000);
        LOG("Testing inventory...");
        auto add = getFuncProvider("inventory.addItems");
        auto remove = getFuncProvider("inventory.removeItems");
        auto getCount = getFuncProvider("inventory.getCurrentCellCount");
        auto getItem = getFuncProvider("inventory.getCurrentCellItem");

        LOG("Adding 10 stone: " << add({"stone", "10"}).data.at(0) << " left");
        LOG("Adding 20 wood: " << add({"wood", "20"}).data.at(0) << " left");
        LOG("Adding 30 stone: " << add({"stone", "30"}).data.at(0) << " left");
        LOG("Current cell contains " << getCount({}).data.at(0) << " " << getItem({}).data.at(0));
        LOG("Removing 5 stone: " << remove({"stone", "5"}).data.at(0) << " removed");
        LOG("Current cell contains " << getCount({}).data.at(0) << " " << getItem({}).data.at(0));
        LOG("Removing 5 wood: " << remove({"wood", "5"}).data.at(0) << " removed");
        LOG("Removing 80 wood: " << remove({"wood", "80"}).data.at(0) << " removed");
        LOG("Removing 5 stone: " << remove({"stone", "5"}).data.at(0) << " removed");
        LOG("Current cell contains " << getCount({}).data.at(0) << " " << getItem({}).data.at(0));
        LOG("Removing 80 stone: " << remove({"stone", "80"}).data.at(0) << " removed");
        LOG("Current cell contains " << getCount({}).data.at(0) << " " << getItem({}).data.at(0));
        LOG("Finished testing inventory");
    } catch (const std::runtime_error& e) {
        LOG("Error while testing inventory: " << e.what());
        LOG("It seems that it does not work properly");
    }

    GameObjCube object = graphicsCreateCube();

    std::vector<GameObject> staticCubes;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 2; ++j) {
            staticCubes.push_back(graphicsCreateCube());
            staticCubes.back().setPosition(GamePosition(i * 20, j * 20, 0));
            staticCubes.back().setPhysicsEnabled(true);
        }
    }
    graphicsAddTexture(object, graphicsLoadTexture("textures/cube1.png"));

    auto tex2 = graphicsLoadTexture("textures/cube2.png");
    for (auto& cube : staticCubes) {
        graphicsAddTexture(cube, tex2);
    }

    auto enemyMesh = graphicsLoadMesh("textures/test_mob.dae");
    EnemyId enemyId = enemyManager.createEnemy(
            enemyMesh, graphicsLoadTexture("textures/mobs/test_mob.png"), {60, 60, 60});
    terrainManager.trackMob(enemyId);
    Enemy& enemy = enemyManager.mutableAccessEnemy(enemyId);
    enemy.sceneNode()->setMaterialFlag(EMF_LIGHTING, false);
    enemy.sceneNode()->setPosition({240, 240, 240});
    graphicsEnablePhysics(enemy.sceneNode(), {60, 75, 60});

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
        enemy.ai();

        std::ignore = graphicsGetPlacePosition(player.getPosition(), player.getCameraTarget());
        object.setPosition(GamePosition(sin(i) * 20, cos(i) * 20, (sin(i) + cos(i)) * 20));
        object.setRotation(i * 100, i * 50, i * 20);

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
