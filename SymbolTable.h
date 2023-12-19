#ifndef _SYMBOL_TABLE__
#define _SYMBOL_TABLE__

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include "types.h"
#include <variant>

class SymbolData {
public:

    using Value = std::variant<int, float, char, std::string, bool, std::vector<SymbolData>>;

    enum Flag {
        Variable, Constant, Function
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

    friend std::ostream& operator << (std::ostream&, const SymbolData&);

private:
    std::string _name;
    Type type;
    std::string scope;
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

    void print(std::ostream& out);
private:
    std::unordered_map<std::string, SymbolData> m_table;
};

#endif