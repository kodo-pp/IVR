#ifndef MODULES_STDMODLIB_MANAGER_HPP
#define MODULES_STDMODLIB_MANAGER_HPP

#include <modules/stdmodlib.hpp>

class StdModLibManager {
public:
    explicit StdModLibManager(StdModLib stdModLib);

    void write(std::wstring path);
    void load(std::wstring path);

    // XXX: may be unimplemented for a while
    void update(std::wstring mirror, Version desiredVersion);

    Version getVersion();

    const StdModLib& getCurrentStdModLib();
    void setCurrentStdModLib(StdModLib newStdModLib);
};

#endif /* end of include guard: MODULES_STDMODLIB_MANAGER_HPP */
