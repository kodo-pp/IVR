#ifndef CORE_MEMORY_MANAGER_HPP
#define CORE_MEMORY_MANAGER_HPP

#include <set>

/**
 * Позволяет освобождать память по событию
 *
 * Здесь должно быть полное описание класса, но в машине темно, и мне трудно
 * (и лень) писать комментарии
 */
class MemoryManager {
private:
    std::set <void *> pointersSet;
public:
    bool track(void *);                 // Начать отслеживать указатель ptr
    bool isTracking(void *);            // Вернуть, отслеживается ли указатель ptr
    bool forget(void *);                // Перестать отслеживать ptr

    // не free(void *), чтобы не было путаницы с free() из stdlib
    bool freePtr(void *);               // Освободить и перестать отслеживать указатель ptr
    bool freeAll();                     // Освободить и перестать отслеживать все указатели

    std::set <void *> getPointersSet(); // Вернуть std::set отслеживаемых указателей
};

extern MemoryManager memoryManager;

#endif /* end of include guard: CORE_MEMORY_MANAGER_HPP */
