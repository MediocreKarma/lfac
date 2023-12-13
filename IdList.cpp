#include "IdList.h"
using namespace std;

void IdList::addVar(const char* type, const char*name) {
    IdInfo var = {string(type), string(name)};
    vars.push_back(var);
}


bool IdList::existsVar(const char* var) {
    string strvar = string(var);
     for (const IdInfo& v : vars) {
        if (var == v.name) { 
            return true;
        }
    }
    return false;
}

void IdList::printVars() {
    for (const IdInfo& v : vars) {
        cout << "name: " << v.name << " type:" << v.type << endl; 
     }
}



IdList::~IdList() {
    vars.clear();
}










