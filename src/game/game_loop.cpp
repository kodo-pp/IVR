#include <game/game_loop.hpp>
#include <game/solid_object.hpp>
#include <graphics/graphics.hpp>
#include <core/init.hpp>
#include <log/log.hpp>
#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>

void gameLoop() {
    int fpsCounter = 0;
    double oneSecondCounter = 0.0;

    int desiredFps = 60; // Desired, but unreachable in practice
    double timeForFrame = 1.0 / desiredFps;

    GameObjCube object = graphicsCreateCube();
    graphicsAddTexture(object, graphicsLoadTexture(L"textures/cube1.png"));
    double i = 0;

    while (true) {
        auto timeBefore = std::chrono::high_resolution_clock::now();
        if (doWeNeedToShutDown) {
            return;
        }
        object.setPosition(GamePosition(sin(i) * 20, cos(i) * 20, (sin(i) + cos(i)) * 20));
        object.setRotation(i * 100, i * 50, i * 20);

        graphicsDraw();
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
