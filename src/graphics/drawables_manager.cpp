#include <modbox/graphics/graphics.hpp>

irr::scene::ISceneNode* DrawablesManager::access(uint64_t handle)
{
    try {
        return drawables.access(handle);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error(std::string("No such drawable handle: ") + std::to_string(handle));
    }
}

uint64_t DrawablesManager::track(irr::scene::ISceneNode* drawable)
{
    return drawables.insert(drawable);
}

std::optional<uint64_t> DrawablesManager::reverseLookup(irr::scene::ISceneNode* drawable)
{
    for (auto& [handle, value] : drawables) {
        if (value == drawable) {
            return handle;
        }
    }
    return {};
}

void DrawablesManager::forget(uint64_t handle)
{
    drawables.remove(handle);
}

DrawablesManager drawablesManager;
