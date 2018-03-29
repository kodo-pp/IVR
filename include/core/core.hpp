#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <vector>
#include <string>
#include <functional>

struct FuncResult {
    void* data;
    int exitStatus;
};

class FuncProvider {
public:
    FuncProvider();
    FuncProvider(std::wstring, std::function <struct FuncResult * (const std::vector <void *> &)>);
    std::wstring getCommand();
    struct FuncResult * operator()(const std::vector <void *> &);
    ~FuncProvider();
private:
    std::wstring command;
    std::function <struct FuncResult * (const std::vector <void *> &)> func;
};

// [no utf-8]
bool initilaizeCore(std::vector <std::string> *);

bool registerFuncProvider(FuncProvider*);
FuncProvider* getFuncProvider(const std::wstring&);

#endif /* end of include guard: CORE_CORE_HPP */
