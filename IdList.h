#include <iostream>
#include <vector>
#include <string>

using namespace std;
struct IdInfo {
    string type;
    string name;
};

class IdList {
    vector<IdInfo> vars;
   
    public:
    bool existsVar(const char* s);
    void addVar(const char* type, const char* name );
    void printVars();
    ~IdList();
};

/*

std::cout << (x += 3) + (x += 5);

*/


