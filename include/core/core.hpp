#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <vector>
#include <string>
#include <functional>

using ArgsSpec = std::string;

struct FuncResult {
    void* data;
    int exitStatus;
};

class FuncProvider {
public:
    FuncProvider();
    FuncProvider(std::string, std::function <struct FuncResult * (const std::vector <void *> &)>);
    std::string getCommand();
    struct FuncResult * operator()(const std::vector <void *> &);
    ~FuncProvider();
private:
    std::string command;
    std::function <struct FuncResult * (const std::vector <void *> &)> func;
};

// [no utf-8]
bool initilaizeCore(std::vector <std::string> *);

bool registerFuncProvider(FuncProvider*, ArgsSpec);
FuncProvider* getFuncProvider(const std::string&);
ArgsSpec getArgsSpec(const std::string&);

#endif /* end of include guard: CORE_CORE_HPP */
