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
    return scope + name;
}

bool Scope::encompassingScope(const std::string& active, const std::string& encompassing) {
    return active.find(encompassing) != std::string::npos;
}

SymbolData::SymbolData(const std::string& scope, const std::string& name, const TypeNms::Type t, const Flag f, const size_t size, const std::string& className) {
    // trebe cumva si numele clasei dat tho ca sa stie de care tip de clasa e
    _scope = scope;
    _name = name;
    type = t;
    _isArray = size > 0;
    if (_isArray) {
        value.resize(size, SymbolData(scope, "", t, f));
    }
    
    _isInit = false;
    switch (f) {
        case Constant:
            _isConst = true;
            break;
        case Function:
            _isFunc = true;
            break;
        case Class:
            _isClassDef = true;
            break;
    }
    if (t == CUSTOM) {
        _className = className;
    }
    else if (!className.empty()) {
        throw std::invalid_argument("Custom class name given for non-class-instance type symbol");
    }
}

SymbolData::SymbolData(const SymbolData& rhs) :
    _name(rhs._name), type(rhs.type), _scope(rhs._scope), _isInit(rhs._isInit), 
    _isConst(rhs._isConst), _isFunc(rhs._isFunc), value(rhs.value) {}

SymbolData::SymbolData(SymbolData&& rhs) :
    _name(std::move(rhs._name)), type(rhs.type), _scope(std::move(rhs._scope)), _isInit(rhs._isInit), 
    _isConst(rhs._isConst), _isFunc(rhs._isFunc), value(std::move(rhs.value)) {}

SymbolData& SymbolData::operator = (const SymbolData& rhs) {
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
        throw std::invalid_argument("Value unassignable");
    }
    else {
        _isInit = true;
        value.emplace_back(_value);
    }
    return *this;
}

bool SymbolData::assignable(const Value& _value) {
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
            return std::holds_alternative<SymbolData>(_value);
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
            return std::holds_alternative<SymbolData>(_value[0]);
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

SymbolTable& SymbolTable::add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, const size_t size, const std::string& className) {
    _table.emplace(Scope::scopeWithNameToString(currentScope(), name), SymbolData(currentScope(), name, type, flag, size, className));
    std::cout << "Adding variable: " << Scope::scopeWithNameToString(currentScope(), name) + "\n";
    return *this;
}

SymbolTable& SymbolTable::addClass(const std::string& name) {
    _classesTable.emplace(name, SymbolData(currentScope(), name, Type::CUSTOM, SymbolData::Flag::Class, 0));
    return *this;
}

SymbolTable& SymbolTable::remove(const SymbolData& data) {
    std::cout << "Removed element " << data.scope() + data.name() << '\n';
    _table.erase(data.scope() + data.name());
    return *this;
}

bool SymbolTable::contains(const std::string& name) {
    return _table.contains(Scope::scopeWithNameToString(currentScope(), name));
}

void SymbolTable::print(std::ostream& out) {
    for (const auto& [path, smb] : _table) {
        out << smb << '\n';
    }
}

std::string SymbolTable::currentScope() {
    return Scope::scopeToString(_currentScopeHierarchy);
}

void SymbolTable::enterAnonymousScope() {
    static size_t i = 0;
    if (i == SIZE_MAX) {
        // reached scope limit for program (infeasible)
        throw std::runtime_error("Reached anon scope limit for program");
    }
    std::string scope = std::to_string(i++);
    _currentScopeHierarchy.push_back(scope);
    std::cout << "Current scope: " << currentScope() << '\n';
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

SymbolData* SymbolTable::find(const std::string& scopedName) {
    auto it = _table.find(scopedName);
    if (it != _table.end()) {
        return &it->second;
    }
    return nullptr;
}

SymbolData* SymbolTable::findClass(const std::string& name) {
    auto it = _classesTable.find(name);
    if (it != _classesTable.end()) {
        return &it->second;
    }
    return nullptr;
}