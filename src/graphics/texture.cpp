#include <graphics/graphics.hpp>
#include <graphics/texture.hpp>
#include <iostream>
#include <irrlicht.h>
#include <log/log.hpp>
#include <mutex>
#include <util/handle_storage.hpp>

using irr::video::ITexture;

std::recursive_mutex textureMutex;
static HandleStorage<uint64_t, ITexture*> textureStorage;

uint64_t registerTexture(ITexture* texture) {
    std::lock_guard<std::recursive_mutex> guard(textureMutex);
    return textureStorage.insert(texture);
}

ITexture* accessTexture(uint64_t handle) {
    std::lock_guard<std::recursive_mutex> guard(textureMutex);
    return textureStorage.access(handle);
}

void removeTexture(uint64_t handle) {
    std::lock_guard<std::recursive_mutex> guard(textureMutex);
    textureStorage.remove(handle);
}
