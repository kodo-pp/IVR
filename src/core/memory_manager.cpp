#include <cstdlib>
#include <set>

#include <core/memory_manager.hpp>

/**
 * Добавляет укаатель во множество отслеживаемых
 *
 * @param ptr: Указаттель для отслеживания
 * @return false, если ptr == nullptr или уже отслеживается
 * @return true в остальных случаях
 */
bool MemoryManager::track(void* ptr)
{
    if (ptr == nullptr) {
        return false;
    }

    if (pointersSet.count(ptr)) {
        return false;
    }

    pointersSet.insert(ptr);
    return true;
}

/**
 * Отвечает, отслеживается ли данный указатель или нет
 *
 * @param ptr: Указаттель
 * @return false, если ptr == nullptr или не отслеживается
 * @return true, если ptr отслеживается
 */
bool MemoryManager::isTracking(void* ptr)
{
    if (ptr == nullptr) {
        return false;
    }

    return (bool)pointersSet.count(ptr);
}

// Мне лень писать документацию, Halp me pls.

bool MemoryManager::forget(void* ptr)
{
    if (ptr == nullptr) {
        return false;
    }

    if (!isTracking(ptr)) {
        return false;
    }

    pointersSet.erase(ptr);
    return true;
}

bool MemoryManager::freePtr(void* ptr)
{
    if (ptr == nullptr) {
        return false;
    }

    if (!isTracking(ptr)) {
        return false;
    }

    free(ptr);
    forget(ptr);
    return true;
}

bool MemoryManager::freeAll()
{
    bool success = true;
    for (auto& ptr : pointersSet) {
        success = success && freePtr(ptr);
    }
    return success;
}

std::set<void*> MemoryManager::getPointersSet()
{
    return pointersSet;
}

MemoryManager memoryManager;
