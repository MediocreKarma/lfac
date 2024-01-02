#include "SymbolTable.h"

using namespace TypeNms;

std::string Scope::scopeToString(const std::vector<std::string>& scope) {
    const char* const delim = "/";
    std::ostringstream result;
    std::copy(scope.begin(), scope.end(),
            std::ostream_iterator<std::string>(result, delim));

    return result.str();
}

std::string Scope::scopeWithNameToString(const std::string& scope, const std::string& name) {
    return scope + name + "/";
}

bool Scope::encompassingScope(const std::string& active, const std::string& encompassing) {
    return active.find(encompassing) != std::string::npos;
}

SymbolData::SymbolData(const std::string& name, const TypeNms::Type t, const Flag f) {
    _name = name;
    type = t;
    _isArray = false;
    _isInit = false;
    switch (f) {
        case Constant:
            _isConst = true;
            break;
        case Function:
            _isFunc = true;
            break;
    }
}

SymbolData::SymbolData(const SymbolData& rhs) :
    _name(rhs._name), type(rhs.type), _scope(rhs._scope), _isInit(rhs._isInit), 
    _isConst(rhs._isConst), _isFunc(rhs._isFunc), value(rhs.value) {}

SymbolData::SymbolData(SymbolData&& rhs) :
    _name(std::move(rhs._name)), type(rhs.type), _scope(std::move(rhs._scope)), _isInit(rhs._isInit), 
    _isConst(rhs._isConst), _isFunc(rhs._isFunc), value(std::move(rhs.value)) {}

SymbolData& SymbolData::operator = (const SymbolData& rhs) {
    // intrebare mai de necunoscatori: astea nu se faceau automat?
    _name = rhs._name;
    type = rhs.type;
    _scope = rhs._scope;
    _isInit = rhs._isInit;
    _isConst = rhs._isConst;
    _isArray = rhs._isArray;
    _isFunc = rhs._isFunc;
    value = rhs.value;
    return *this;
}

SymbolData& SymbolData::operator = (SymbolData&& rhs) {
    _name = std::move(rhs._name);
    type = rhs.type;
    _scope = std::move(rhs._scope);
    _isInit = rhs._isInit;
    _isConst = rhs._isConst;
    _isArray = rhs._isArray;
    _isFunc = rhs._isFunc;
    value = std::move(rhs.value);
    return *this;
}

SymbolData& SymbolData::assign(const Value& _value) {
    if (!assignable(_value)) { 
        throw std::invalid_argument("invalid dar de ce? throw in assignable?");
    }
    else {
        _isInit = true;
        _isArray = false;
        value.emplace_back(_value);
    }
    return *this;
}

bool SymbolData::assignable(const Value& _value) {
    // nici nu stiu ce vrea sa exprime aceasta functie... nu ma ating de ea momentan
    // presupun ca vrei sa vezi daca poti sa scoti valoarea dintr un id... sau ceva
    switch (type) {
        case INT:
            return std::holds_alternative<int>(_value);
        case FLOAT:
            return std::holds_alternative<float>(_value);
        case CHAR:
            return std::holds_alternative<char>(_value);
        case STRING:
            return std::holds_alternative<std::string>(_value);
        case BOOL:
            return std::holds_alternative<bool>(_value);
        case CUSTOM: // mai multe verificari aici ?
            return std::holds_alternative<std::vector<SymbolData>>(_value);
    }
    // throw sau yyerror si return false; nu stiu inca
    if (_isArray) {
        return false;
    }
    if (_isConst) {
        return false;
    }
    if (_isFunc) {
        return false;
    }
    return true;
}

bool SymbolData::assignable(const std::vector<Value>& _value) {
    if (_value.size() != value.size()) {
        return false;
    }
    switch (type) {
        case INT:
            return std::holds_alternative<int>(_value[0]);
        case FLOAT:
            return std::holds_alternative<float>(_value[0]);
        case CHAR:
            return std::holds_alternative<char>(_value[0]);
        case STRING:
            return std::holds_alternative<std::string>(_value[0]);
        case BOOL:
            return std::holds_alternative<bool>(_value[0]);
        case CUSTOM: // mai multe verificari aici ?
            return std::holds_alternative<std::vector<SymbolData>>(_value[0]);
    }
    // throw sau yyerror si return false; nu stiu inca
    if (!_isArray) {
        return false;
    }
    if (_isConst) {
        return false;
    }
    if (_isFunc) {
        return false;
    }
    return true;
}

std::string SymbolData::name() const {
    return _name;
}

std::string SymbolData::scope() const {
    return _scope;
}

std::ostream& operator << (std::ostream& out, const SymbolData& sd) {
    // momentan atat
    out << typeToStr(sd.type) << ' ' << sd.name();
    return out;
}


SymbolTable& SymbolTable::add(const SymbolData& data) {
    // current scope
    // todo: add more types of scopes here
    // -- am scos si _var_ pentru nu i ca si cum vreau sa pot sa am de exemplu o functie si o variabila si un constant cu acelasi nume in acelasi scope.
    // vreau sa fie toate Simboluri non-re-declarabile
    m_table.emplace(Scope::scopeWithNameToString(currentScope(), data.name()), data);
    std::cout << "Adding variable: " << Scope::scopeWithNameToString(currentScope(), data.name()) + "\n";
    return *this;
}

bool SymbolTable::contains(const std::string& name) {
    return m_table.contains(Scope::scopeWithNameToString(currentScope(), name));
}

void SymbolTable::print(std::ostream& out) {
    for (const auto& [path, smb] : m_table) {
        out << smb << '\n';
    }
}

std::string SymbolTable::currentScope() {
    return Scope::scopeToString(_currentScopeHierarchy);
}

// todo: specify if i'm entering a class scope or a fxn scope. We might need that info
void SymbolTable::enterScope(const std::string& str) {
    _currentScopeHierarchy.push_back(str);
    std::cout << "Current scope: " << currentScope() << "\n";
}

void SymbolTable::exitScope() {
    _currentScopeHierarchy.pop_back();
    std::cout << "Current scope: " << currentScope() << "\n";
}