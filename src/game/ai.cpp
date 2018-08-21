#include <game/ai.hpp>

static GamePosition defaultAiFunc()
{
    return {500, 200, 500};
}

std::function<GamePosition(void)> getDefaultAiFunc()
{
    return defaultAiFunc;
}
