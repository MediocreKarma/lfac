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
}

class SymbolData {
public:

    using Value = std::variant<int, float, char, std::string, bool, std::vector<SymbolData>>;

    enum Flag {
        Variable, Constant, Function
    };

    enum RandomizedScopes {
        For,
        While,
        DoWhile,
        If,
        Else /*is the scope of an else part of an if? eu zic ca s separate dar ce i drept nici n-am habar*/
    };

    SymbolData(const std::string& name, Type type, Flag flag);
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

private:
    std::string _name;
    Type type;
    std::string _scope;
    bool _isInit;
    bool _isConst;
    bool _isArray;
    bool _isFunc;
    std::vector<std::variant<int, float, char, std::string, bool, std::vector<SymbolData>>> value;

    bool assignable(const Value& _value);
    bool assignable(const std::vector<Value>& _value);
};


class SymbolTable {
public:
    SymbolTable& add(const SymbolData& data);
    //SymbolTable& add(SymbolData&& data);
    bool contains(const std::string& name);

    void print(std::ostream& out = std::cout);
    std::string currentScope();
    void enterScope(const std::string&);
    void exitScope();
private:

    std::unordered_map<std::string, SymbolData> m_table;

    std::vector<std::string> _currentScopeHierarchy;
};

#endif