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

// poate un namespace unde sa definim de exemplu regulile pe baza carora se construiesc/deconstruiesc scope names ar fi cute

namespace Scope {
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

    using Value = std::variant<int, float, char, std::string, bool, std::vector<SymbolData>>;

    enum Flag {
        Variable, Constant, Function
    };

    SymbolData(const std::string& name, TypeNms::Type type, Flag flag);
    //SymbolData(std::string&& name, Type type, Flag flag);
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
    std::vector<std::variant<int, float, char, std::string, bool, std::vector<SymbolData>>> value;

    using base_var = std::variant<int, bool, char, std::string, float>;

    bool assignable(const Value& _value);
    bool assignable(const std::vector<Value>& _value);
};


class SymbolTable {

public:
    enum RandomizedScopes {
        For,
        While,
        DoWhile,
        If,
        Else,
        COUNT // doar ca sa stiu cate scopes au fost definite
    };
    SymbolTable& add(const SymbolData& data);
    //SymbolTable& add(SymbolData&& data);
    bool contains(const std::string& name);

    void print(std::ostream& out = std::cout);
    std::string currentScope();
    void enterScope(RandomizedScopes scope);
    void enterScope(const std::string&);
    void exitScope();
private:
    static std::string randomizedScopeToStr(RandomizedScopes scope);
    std::array<size_t, RandomizedScopes::COUNT> _randomizedScopeCounts;
    
    std::unordered_map<std::string, SymbolData> m_table;

    // si trebuie sa tinem undeva si clasele definite so far
    
    std::vector<std::string> _currentScopeHierarchy;
};

#endif