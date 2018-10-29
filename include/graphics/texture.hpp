#ifndef GRAPHICS_TEXTURE_HPP
#define GRAPHICS_TEXTURE_HPP

#include <cstdint>
#include <cstdlib>

#include <irrlicht_wrapper.hpp>

using irr::video::ITexture;

uint64_t registerTexture(ITexture* texture);
ITexture* accessTexture(uint64_t handle);
void removeTexture(uint64_t handle);

#endif /* end of include guard: GRAPHICS_TEXTURE_HPP */
