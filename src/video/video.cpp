#include <video/video.h>
#include <irrlicht.h>
#include <vector>

bool initializeVideo(std::vector <std::string> * args) {
    if (args == nullptr) {
        return false;
    }
    video::irrdev = irr::createDevice()
}

namespace video {
    irr::IrrlichtDevice * irrdev = nullptr;
};
