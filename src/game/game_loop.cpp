#include <game/game_loop.hpp>
#include <game/solid_object.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <iostream>
#include <string>
#include <unistd.h>

void gameLoop() {
    ISceneNode* object = graphicsCreateCube();
    graphicsAddTexture(object, graphicsLoadTexture(L"/home/kodopp/a.png"));
    double i = 0;
    while (true) {
        graphicsMoveObject(object, sin(i) * 20, cos(i) * 20, (sin(i) + cos(i)) * 20);
        graphicsDraw();
        usleep(10000);
        i += 0.01;
    }
}
