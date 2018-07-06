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

std::recursive_mutex irrlichtMutex;

void gameLoop() {
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

    while (true) {
        graphicsMoveCameraDelta(0.1, 0, 0);
        graphicsRotateCameraDelta(0, 1, 0);
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
