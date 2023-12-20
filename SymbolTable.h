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
    // cat de ridicol ar fi sa fie mai degraba unordered_map<string, unordered_map<string, SymbolData>>? cheia1 ar fi numele variabilei, cheia2 ar fi scope-ul variabilei reprezentat ca un string
    // as face asta da nuj cat de costisitor e
    // also se poate face si unordered_map<string, vector<string, SymbolData>> (cat de des declari pana la urma "x" in scope-uri diferite?) dar mna. Cum Crezi
    std::unordered_map<std::string, SymbolData> m_table;

    // si trebuie sa tinem undeva si clasele definite so far
    
    std::vector<std::string> _currentScopeHierarchy;
};

#endif