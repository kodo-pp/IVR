#include <game/game_loop.hpp>
#include <game/solid_object.hpp>
#include <graphics/graphics.hpp>
#include <core/init.hpp>
#include <log/log.hpp>
#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>
#include <irrlicht.h>
#include <unordered_set>
#include <memory>
#include <vector>
#include <cmath>

std::recursive_mutex irrlichtMutex;

static void processKeys() {
    // XXX: This is stub, camera movement and rotation should be done by class like Player
    const IrrKeyboardEventReceiver& receiver = getKeyboardEventReceiver();

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
        double speed = 3.0;
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

        graphicsMoveCameraForward(speed, directionOffset);
    }

    // Camera movement (vertical)
    {
        int dy = 0;
        if (receiver.isKeyPressed(irr::KEY_SPACE)) {
            ++dy;
        }
        if (receiver.isKeyPressed(irr::KEY_LSHIFT)) {
            --dy;
        }
        graphicsMoveCameraDelta(0, 2.0 * dy, 0);
    }

    // Camera rotation
    {
        int dx = 0, dy = 0;
        if (receiver.isKeyPressed(irr::KEY_UP)) {
            --dx;
        }
        if (receiver.isKeyPressed(irr::KEY_RIGHT)) {
            ++dy;
        }
        if (receiver.isKeyPressed(irr::KEY_DOWN)) {
            ++dx;
        }
        if (receiver.isKeyPressed(irr::KEY_LEFT)) {
            --dy;
        }

        const double speed = 2.0;
        irr::core::vector3df delta(speed * dx, speed * dy, 0);
        graphicsRotateCameraDelta(delta);
    }
}

void gameLoop() {
    graphicsLoadTerrain("textures/heightmap.png");
    int fpsCounter = 0;
    double oneSecondCounter = 0.0;

    int desiredFps = 30; // Desired, but unreachable in practice
    double timeForFrame = 1.0 / desiredFps;

    GameObjCube object = graphicsCreateCube();

    std::vector <GameObjCube> staticCubes;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            staticCubes.push_back(graphicsCreateCube());
            staticCubes.back().setPosition(GamePosition(i * 20, j * 20, 0));
        }
    }
    graphicsAddTexture(object, graphicsLoadTexture(L"textures/cube1.png"));

    auto tex2 = graphicsLoadTexture(L"textures/cube2.png");
    for (auto& cube : staticCubes) {
        graphicsAddTexture(cube, tex2);
    }

    double i = 0;

    while (irrDeviceRun()) {
        processKeys();
        //graphicsMoveCameraDelta(0.1, 0, 0);
        //graphicsRotateCameraDelta(0, 1, 0);
        auto timeBefore = std::chrono::high_resolution_clock::now();
        if (doWeNeedToShutDown) {
            return;
        }
        object.setPosition(GamePosition(sin(i) * 20, cos(i) * 20, (sin(i) + cos(i)) * 20));
        object.setRotation(i * 100, i * 50, i * 20);

        {
            std::lock_guard <std::recursive_mutex> lock(irrlichtMutex);
            graphicsDraw();
        }
        ++fpsCounter;

        auto timeAfter = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast < std::chrono::duration<double> > (timeAfter - timeBefore);
        double timeToSleep = timeForFrame - duration.count();
        if (oneSecondCounter > 1.0) {
            fpsCounter = 0;
            oneSecondCounter = 0.0;
        }
        if (timeToSleep < 0.0) {
            LOG("Warning: frame rendering took longer than 1 / " << desiredFps << " s");
            LOG("Time to sleep is " << timeToSleep);
        } else {
            usleep(static_cast <int> (timeToSleep * 1e+6));
        }

        auto timeAfterSleep = std::chrono::high_resolution_clock::now();
        auto fullDuration = std::chrono::duration_cast < std::chrono::duration<double> > (timeAfterSleep - timeBefore);
        oneSecondCounter += fullDuration.count();

        i += timeForFrame;
    }
}
