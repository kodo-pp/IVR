#include <map>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <core/core.hpp>
#include <misc/die.hpp>
#include <iostream>

//std::map <std::string, FuncProvider*> funcProviderMap;

static void initializeFuncProviderMap(std::vector <std::string> * args) {

}

bool initilaizeCore(std::vector <std::string> * args) {
    try {
        initializeFuncProviderMap(args);
    } catch (std::runtime_error e) {
        errdie("unable to initialize function provider map", e.what());
    }
}

//bool registerFuncProvider(FuncProvider* prov) {
//
//}
