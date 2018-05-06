#include <game/game_loop.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <iostream>
#include <unistd.h>

void gameLoop() {
    ISceneNode* object = graphicsCreateObject(L"/home/kodopp/monkey.obj");
    double i = 0;
    while (true) {
        graphicsMoveObject(object, sin(i) * 20, cos(i) * 20, sin(i) * cos(i) * 20);
        graphicsDraw();
        usleep(10000);
        i += 0.01;
    }
}
