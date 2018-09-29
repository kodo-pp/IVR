#include <iostream>
#include <mutex>

#include <modbox/graphics/graphics.hpp>
#include <modbox/graphics/texture.hpp>
#include <modbox/log/log.hpp>
#include <modbox/util/handle_storage.hpp>

#include <irrlicht.h>

using irr::video::ITexture;

std::recursive_mutex textureMutex;
static HandleStorage<uint64_t, ITexture*> textureStorage;

uint64_t registerTexture(ITexture* texture)
{
    std::lock_guard<std::recursive_mutex> guard(textureMutex);
    return textureStorage.insert(texture);
}

ITexture* accessTexture(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> guard(textureMutex);
    return textureStorage.access(handle);
}

void removeTexture(uint64_t handle)
{
    std::lock_guard<std::recursive_mutex> guard(textureMutex);
    textureStorage.remove(handle);
}
