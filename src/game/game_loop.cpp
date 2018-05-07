#include <game/game_loop.hpp>
#include <game/solid_object.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <iostream>
#include <string>
#include <unistd.h>

void gameLoop() {
    //ISceneNode* object = graphicsCreateObject(L"/home/kodopp/monkey.obj");
    GameObject object(L"/home/kodopp/monkey.obj");
    double elapsedTime = 0;
    while (true) {
        object.setPosition(GamePosition(sin(elapsedTime) * 20, 0, 0));
        graphicsDraw();
        usleep(10000);
        elapsedTime += 0.01;
    }
}
