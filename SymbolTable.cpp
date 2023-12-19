#include "SymbolTable.h"

std::string Scope::scopeToString(const std::vector<std::string>& scope) {
    const char* const delim = "/";
    std::ostringstream result;
    std::copy(scope.begin(), scope.end(),
            std::ostream_iterator<std::string>(result, delim));

    return result.str();
}

Type typeFromString(const std::string& str) {
    // :)
    return std::unordered_map<std::string, Type> {
        {"int", INT},
        {"float", FLOAT},
        {"char", CHAR},
        {"string", STRING},
        {"bool", BOOL},
    }.at(str);
}

SymbolData::SymbolData(const std::string& name, const Type t, const Flag f) {
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
    std::string outType = "UNDEFINED";
    switch (sd.type) {
        case INT:
            outType = "Int";
            break;
        case FLOAT:
            outType = "Float";
            break;
        case CHAR:
            outType = "Char";
            break;
        case STRING:
            outType = "String";
            break;
        case BOOL:
            outType = "Bool";
            break;
        case CUSTOM:
            outType = "User defined class";
            break;
    }
    // momentan atat
    out << outType << ' ' << sd.name();
    return out;
}


SymbolTable& SymbolTable::add(const SymbolData& data) {
    // current scope
    // todo: actually do scopes here...  also for fxns and stuff
    // also todo: standardize adding something to a scope string with helper fxns
    m_table.emplace(currentScope() + data.name() + "/", data);
    std::cout << "Adding variable: " << currentScope() + "_var_" + data.name() + "\n";
    return *this;
}

bool SymbolTable::contains(const std::string& name) {
    // cred ca aici vrem defapt sa dam direct un string care sa reprezinte scope-ul. Exista Variabila in scope-ul curent?
    // saaaaau un vector<string> care sa mi fie defapt ierarhia... nu stiu
    return m_table.contains(name);
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