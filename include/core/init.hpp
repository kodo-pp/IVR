#ifndef CORE_INIT_HPP
#define CORE_INIT_HPP

#include <atomic>
#include <string>
#include <vector>

extern std::atomic<bool> doWeNeedToShutDown;

// [no utf-8]
void init(std::vector<std::string>& args);

#endif /* end of include guard: CORE_INIT_HPP */
