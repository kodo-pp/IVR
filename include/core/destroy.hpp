#ifndef CORE_DESTROY_HPP
#define CORE_DESTROY_HPP

#include <atomic>
#include <mutex>

extern std::atomic<bool> doWeNeedToShutDown;
extern std::atomic<bool> areWeShuttingDown;

void destroy(void);
void sigIntHandler(int);

#endif /* end of include guard: CORE_DESTROY_HPP */
