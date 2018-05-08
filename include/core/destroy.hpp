#ifndef CORE_DESTROY_HPP
#define CORE_DESTROY_HPP

#include <atomic>

extern std::atomic <bool> doWeNeedToShutDown;

void destroy(void);
void sigIntHandler(int);

#endif /* end of include guard: CORE_DESTROY_HPP */
