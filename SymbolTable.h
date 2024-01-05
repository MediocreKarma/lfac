#ifndef _SYMBOL_TABLE__
#define _SYMBOL_TABLE__

#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <variant>

#include "types.h"

namespace Scope { // scope utils
    static const char* DELIM = "/";
    std::string scopeToString(const std::vector<std::string>&);
    std::string scopeWithNameToString(const std::string& scope, const std::string& name);
    // verify if access to variable in encompassing scope
    // is possible in active scope 
    // i.e. scope is contained by the other scope
    bool encompassingScope(const std::string& active, const std::string& encompassing); 
}

class SymbolData {
public:

    using Value = std::variant<int, float, char, std::string, bool, SymbolData>;

    enum Flag {
        Variable, Constant, Function, Class // eu zic ca trebe
    };

    SymbolData(const std::string& scope, const std::string& name, TypeNms::Type type, Flag flag, size_t size = 0, const std::string& className = "");
    SymbolData(const SymbolData&);
    SymbolData(SymbolData&&);
    SymbolData& operator = (const SymbolData&);
    SymbolData& operator = (SymbolData&&);
    
    SymbolData& assign(const Value& value);
    SymbolData& assign(Value&& value);
    SymbolData& assign(const std::vector<Value>& values);
    SymbolData& assign(std::vector<Value>&& values);

    std::string name() const;
    std::string scope() const;

    friend std::ostream& operator << (std::ostream&, const SymbolData&);

    // cu siguranta astea functioneaza cum trebuie by default
    friend bool operator == (const SymbolData& a, const SymbolData& b) = default;
    friend bool operator != (const SymbolData& a, const SymbolData& b) = default;
private:

    std::string _name;
    TypeNms::Type type;
    std::string _scope;
    bool _isInit;
    bool _isConst;
    bool _isArray;
    bool _isFunc;
    bool _isClassDef; // daca ai un nume mai bun pt definiri de clase...

    // pt classInstances ca sa stie ce clasa is daca-s tip custom... ca ne trebuie cand afisam type ul in symboltable
    std::string _className;

    std::vector<Value> value;

    using base_var = std::variant<int, bool, char, std::string, float>;

    bool assignable(const Value& _value);
    bool assignable(const std::vector<Value>& _value);
};


class SymbolTable {

public:
    SymbolTable& add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, size_t size = 0, const std::string& className = "");
    SymbolTable& remove(const SymbolData& data);
    //SymbolTable& add(SymbolData&& data);
    bool contains(const std::string& name);

    SymbolTable& addClass(const std::string& name);

    void print(std::ostream& out = std::cout);
    std::string currentScope();
    void enterScope(const std::string&);
    void enterAnonymousScope();
    void exitScope();

    SymbolData* find(const std::string& scopedName);
    SymbolData* findClass(const std::string& name);

private:
    
    std::unordered_map<std::string, SymbolData> _table;
    std::unordered_map<std::string, SymbolData> _classesTable;
    std::vector<std::string> _currentScopeHierarchy;
};

#endif