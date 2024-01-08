#pragma once

#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <variant>
#include <memory>

#include "types.h"

namespace Scope { // scope utils
    static const char* DELIM = "/";
    std::string scopeToString(const std::vector<std::string>& scope, ssize_t size = -1);
    std::string scopeWithNameToString(const std::string& scope, const std::string& name);
    // verify if access to variable in encompassing scope
    // is possible in active scope 
    // i.e. scope is contained by the other scope
    bool encompassingScope(const std::string& active, const std::string& encompassing); 
}

class AST;

class SymbolData {
public:

    using Value = std::variant<int, float, char, std::string, bool, std::vector<SymbolData>>;

    enum Flag {
        Variable, Constant, Function, Class // eu zic ca trebe
    };

    SymbolData() = default;
    SymbolData(const std::string& scope, const std::string& name, TypeNms::Type type, Flag flag, size_t size = 0, const std::string& className = "");
    SymbolData(const std::string& scope, const std::string& name, TypeNms::Type type, Flag flag, const Value& value);
    SymbolData(const SymbolData&) = default;
    SymbolData(SymbolData&&) = default;
    SymbolData& operator = (const SymbolData&) = default;
    SymbolData& operator = (SymbolData&&) = default;
    
    SymbolData& setConst(bool value = true);
    SymbolData& setType(TypeNms::Type type);
    SymbolData& assign(const Value& value);
    SymbolData& assign(const SymbolData& val);

    SymbolData& addSymbolToBeginning(const SymbolData&); // pt functii/clase...?
    SymbolData& addSymbol(const SymbolData&); // pt functii/clase...?
    
    std::string name() const;
    std::string scope() const;
    TypeNms::Type type() const;
    const Value& value() const;
    std::string className() const;

    bool isFunc() const;
    bool isArray() const;
    bool isInit() const;
    bool isConst() const;

    SymbolData* member(const std::string& id);
    SymbolData* member(size_t index);

    SymbolData  instantiateClass(const std::string& scope, const std::string& name) const;

    bool hasSameTypeAs(const SymbolData& sym) const; // has same members if custom... etc

    friend std::ostream& operator << (std::ostream&, const SymbolData&);
    friend void printSubsymbol(std::ostream& out, const SymbolData& sd, size_t depth);

    // cu siguranta astea functioneaza cum trebuie by default
    friend bool operator == (const SymbolData& a, const SymbolData& b) = default;
    friend bool operator != (const SymbolData& a, const SymbolData& b) = default;
    friend bool sameType(const SymbolData& a, const SymbolData&b);
private:

    std::string _name;
    TypeNms::Type _type;
    std::string _scope;
    bool _isInit = false;
    bool _isConst = false;
    bool _isArray = false;
    bool _isFunc = false;
    bool _isClassDef = false;

    // pt classInstances ca sa stie ce clasa is daca-s tip custom...
    // ca ne trebuie cand afisam type ul in symboltable
    std::string _className;

    Value _value;

    void throwWhenUnassignable(const Value& val);
};


class SymbolTable {
public:
    SymbolTable() = default;
    SymbolTable& add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, size_t size = 0, const std::string& className = "");
    SymbolTable& add(const SymbolData& data);
    SymbolTable& remove(const SymbolData& data);
    bool contains(const std::string& name);

    SymbolTable& addClass(const std::string& name);

    void print(std::ostream& out = std::cout);
    std::string currentScope();
    void enterScope(const std::string&);
    void enterAnonymousScope();
    void exitScope();

    SymbolData* find(const std::string& scopedName);
    SymbolData* findId(const std::string& id);
    SymbolData* findClass(const std::string& name);

private:
    
    std::unordered_map<std::string, std::unique_ptr<SymbolData>> _table;
    std::unordered_map<std::string, std::unique_ptr<SymbolData>> _classesTable;
    std::vector<std::string> _currentScopeHierarchy = {""};
};
