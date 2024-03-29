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
#include <algorithm>

#include "types.h"
#include "Utils.h"

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
        Variable, Constant, Function, Class, InitList
    };

    SymbolData() = default;
    SymbolData(const std::string& scope, const std::string& name, TypeNms::Type type, Flag flag, const std::vector<size_t>& sizes = {}, const SymbolData* classDef = nullptr);
    SymbolData(const std::string& scope, const std::string& name, TypeNms::Type type, Flag flag, const Value& value);
    SymbolData(const SymbolData&) = default;
    SymbolData(SymbolData&&) = default;
    SymbolData& operator = (const SymbolData&) = default;
    SymbolData& operator = (SymbolData&&) = default;
    
    SymbolData& setConst(bool value = true);
    SymbolData& setType(TypeNms::Type type);
    SymbolData& assign(const Value& value);
    SymbolData& assign(const SymbolData& val);

    SymbolData& addSymbolToBeginning(const SymbolData&); // pt functii/clase
    SymbolData& addSymbol(const SymbolData&); // pt functii/clase
    
    std::string name() const;
    std::string scope() const;
    TypeNms::Type type() const;
    const Value& value() const;
    std::string className() const;

    const std::vector<size_t>& sizes() const;

    bool isFunc() const;
    bool isArray() const;
    bool isInit() const;
    bool isConst() const;

    SymbolData* member(const std::string& id);
    SymbolData* member(size_t index);

    SymbolData  instantiateClass(const std::string& scope, const std::string& name) const;

    bool hasSameTypeAs(const SymbolData& sym) const; // has same members if custom... etc

    std::string valueStr() const;
    std::string trueValueStr() const;
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
    bool _isInitList = false;

    // pt classInstances ca sa stie ce clasa is daca-s tip custom...
    // ca ne trebuie cand afisam type ul in symboltable
    std::string _className;
    std::vector<size_t> _sizes;

    Value _value;

    void throwWhenUnassignable(const Value& val);
    void recursiveScopeApply();
};


class SymbolTable {
public:
    SymbolTable() = default;
    SymbolTable& add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, const std::vector<size_t>& sizes = {});
    SymbolTable& add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, const std::vector<size_t>& sizes, const SymbolData* classDef);
    SymbolTable& add(const SymbolData& data);
    SymbolTable& remove(const SymbolData& data);
    SymbolTable& setReturnType(TypeNms::Type t, const std::string& className = "");
    bool contains(const std::string& name);
    bool sameReturnType(TypeNms::Type t, const std::string& className = "");

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
    std::vector<SymbolData*> _orderedTable;
    std::vector<SymbolData*> _orderedClassesTable;
    std::vector<std::string> _currentScopeHierarchy = {""};

    TypeNms::Type _returnType;
    std::string _returnClass;
};
