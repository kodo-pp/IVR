#ifndef CORE_MEMORY_MANAGER_HPP
#define CORE_MEMORY_MANAGER_HPP

#include <cstdlib>
#include <unordered_set>

#include <log/log.hpp>

class MemoryManager
{
public:
    bool track(void* ptr);
    bool forget(void* ptr);
    bool isTracking(void* ptr);

    template <typename T>
    void deletePtr(void* ptr)
    {
        if (!isTracking(ptr)) {
            LOG("WARNING: attempted to delete untracked pointer " << ptr);
            // abort();
            return;
        }
        delete static_cast<T*>(ptr);
        forget(ptr);
    }

    const std::unordered_set<void*>& getPointersSet();

private:
    std::unordered_set<void*> pointersSet;
};

extern MemoryManager memoryManager;

#endif /* end of include guard: CORE_MEMORY_MANAGER_HPP */
