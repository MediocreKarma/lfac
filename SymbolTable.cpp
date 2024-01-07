#include "SymbolTable.h"
#include "AST.h"

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
    _type = t;
    _isArray = (size > 0); // e mai usor de citit daca pui o expr de genu n paranteza
    if (_isArray) {
        _value = std::vector<SymbolData>(size, SymbolData(scope, "", t, f));
    }
    _isInit = false;
    switch (f) {
        case Constant:
            _isConst = true;
            break;
        case Function:
            _isFunc = true;
            _value = std::vector<SymbolData>();
            break;
        case Class:
            _isClassDef = true;
            _value = std::vector<SymbolData>();
            break;
    }
    if (t == CUSTOM) {
        _className = className;
    }
    else if (!className.empty()) {
        throw std::invalid_argument("Custom class name given for non-class-instance type symbol");
    }
}

SymbolData::SymbolData(const std::string& scope, const std::string& name, const TypeNms::Type t, const Flag f, const Value& val) :
    SymbolData(scope, name, t, f) {
    _value = val;
}

SymbolData& SymbolData::setType(TypeNms::Type type) {
    _type = type;
    return *this;
}

SymbolData& SymbolData::assign(const SymbolData& symbol) {
    if (sameType(*this, symbol) == false) {
        std::string from  = TypeNms::typeToStr(symbol.type());
        std::string to    = TypeNms::typeToStr(type());
        size_t thisSize   = std::get<std::vector<SymbolData>>(value()).size();
        size_t symbolSize = std::get<std::vector<SymbolData>>(symbol.value()).size();
        if (type() == CUSTOM) {
            to += ' ' + className();
        }
        if (symbol.type() == CUSTOM) {
            from += ' ' + symbol.className();
        }
        if (isArray()) {
            from += "[" + std::to_string(thisSize) + "]";
        }
        if (symbol.isArray()) {
            to += "[" + std::to_string(symbolSize) + "]";
        }
        throw std::runtime_error("Cannot assign type: " + from + " to type: " + to);
    }
    _isInit = true;
    _value = symbol.value();
    return *this;
}

SymbolData& SymbolData::assign(const Value& val) {
    throwWhenUnassignable(val);
    _isInit = true;
    _value = val;
    return *this;
}

void SymbolData::throwWhenUnassignable(const Value& val) {
    if (isConst()) {
        throw std::runtime_error("Cannot assign to constant variable!");
    }
    if (isFunc()) {
        throw std::runtime_error("Cannot assign to function!");
    }
    std::string valueType = "";
    if (std::holds_alternative<int>(val)) {
        if (type() == INT && isArray() == false) {
            return;
        }
        valueType = TypeNms::typeToStr(INT);
    }
    else if (std::holds_alternative<float>(val)) {
        if (type() == FLOAT && isArray() == false) {
            return;
        }
        valueType = TypeNms::typeToStr(FLOAT);
    }
    else if (std::holds_alternative<char>(val)) {
        if (type() == CHAR && isArray() == false) {
            return;
        }
        valueType = TypeNms::typeToStr(CHAR);
    }
    else if (std::holds_alternative<bool>(val)) {
        if (type() == BOOL && isArray() == false) {
            return;
        }
        valueType = TypeNms::typeToStr(BOOL);
    }
    else if (std::holds_alternative<std::string>(val)) {
        if (type() == STRING && isArray() == false) {
            return;
        }
        valueType = TypeNms::typeToStr(STRING);
    }
    if (valueType.empty() == false) {
        if (isArray() == true) {
            throw std::runtime_error("Cannot assign " + valueType + " to an array identifier");
        }
        if (type() != CUSTOM) {
            throw std::runtime_error("Cannot assign " + valueType + " to an identifier of type " + TypeNms::typeToStr(type()));
        }
        throw std::runtime_error("Cannot assign " + valueType + " to an identifier of type Class " + className());
    }
    // initializer list
    const std::vector<SymbolData>&  thisData = std::get<std::vector<SymbolData>>(value());
    const std::vector<SymbolData>& otherData = std::get<std::vector<SymbolData>>(val);
    if (thisData.size() < otherData.size()) {
        throw std::runtime_error("Initializer list has too many parameters");
    }
    if (isArray()) {
        for (size_t i = 0; i < otherData.size(); ++i) {
            if (!sameType(thisData[0], otherData[i])) {
                throw std::runtime_error("Element " + std::to_string(i) + " of initializer list is of a different type");
            }
        }
    }
    else {
        for (size_t i = 0; i < otherData.size(); ++i) {
            if (!sameType(thisData[i], otherData[i])) {
                if (isArray()) {
                    throw std::runtime_error("Element " + std::to_string(i) + " of initializer list cannot be assigned to member " + thisData[i].name());
                }
            }
        }
    }
}

SymbolData& SymbolData::addSymbol(const SymbolData& symbol) {
    if (!_isFunc and !_isClassDef) {
        throw std::runtime_error("Cannot add symbol to non-class, non-function symbol");
    }
    std::get<std::vector<SymbolData>>(_value).push_back(symbol);
    return *this;
}

std::string SymbolData::name() const {
    return _name;
}

std::string SymbolData::scope() const {
    return _scope;
}

TypeNms::Type SymbolData::type() const {
    return _type;
}

const SymbolData::Value& SymbolData::value() const {
    return _value;
}

std::string SymbolData::className() const {
    return _className;
}

bool SymbolData::isFunc() const {
    return _isFunc;
}

bool SymbolData::isArray() const {
    return _isArray;
}

bool SymbolData::isInit() const {
    return _isInit;
}

bool SymbolData::isConst() const {
    return _isConst;
}

SymbolData SymbolData::instantiateClass(const std::string& scope, const std::string& name) const {
    if (!_isClassDef) {
        throw std::invalid_argument("Cannot clone non-class_template");
    }
    SymbolData symbol(*this);
    symbol._scope = scope;
    symbol._name = name;
    symbol._className = _className;
    return symbol;
}


// for class defs/instances
SymbolData* SymbolData::member(const std::string& id) {
    if (_isArray || _isFunc) {
        return nullptr;
    }
    if (_type != TypeNms::Type::CUSTOM) {
        return nullptr;
    }
    if (std::holds_alternative<std::vector<SymbolData>>(value()) == false) {
        // vals of id are not symbol data
        return nullptr;
    }    
    for (SymbolData& storedSymbol : std::get<std::vector<SymbolData>>(_value)) {
        if (storedSymbol.name() == id) {
            return &storedSymbol;
        }
    }
    return nullptr;
}

SymbolData* SymbolData::member(const size_t index) {
    if (!_isArray) {
        throw std::invalid_argument("Cannot apply index operator to non-array types");
    }
    std::vector<SymbolData>* vec = std::get_if<std::vector<SymbolData>>(&_value);
    if (index >= vec->size()) {
        throw std::invalid_argument("Index out of bounds");
    }
    return &(*vec)[index];
}

bool SymbolData::hasSameTypeAs(const SymbolData& sym) const {
    return sameType(*this, sym);
}

bool sameType(const SymbolData& a, const SymbolData& b) {
    if (a.type() != CUSTOM and !a.isFunc()) { // base variable
        return a.type() == b.type(); 
    }
    if (a.type() != b.type()) {
        return false;
    }
    const auto& aData = std::get<std::vector<SymbolData>>(a.value());
    const auto& bData = std::get<std::vector<SymbolData>>(b.value());
    if (aData.size() != bData.size()) {
        return false;
    }
    if (a.isArray() != b.isArray() || a.isFunc() != b.isFunc()) { // any type of array
        return false;
    }
    for (size_t i = 0; i < aData.size(); ++i) {
        if (!sameType(aData[i], bData[i])) {
            return false;
        }
    }
    return true;
}

void printSubsymbol(std::ostream& out, const SymbolData& sd, size_t depth) {
    for (const SymbolData& subSd : std::get<std::vector<SymbolData>>(sd.value())) {
        out << '\n' << std::string(depth, '\t') << subSd;
        if (subSd.type() == CUSTOM || sd.isArray()) {
            printSubsymbol(out, subSd, depth + 1);
        }
    }
}

std::ostream& operator << (std::ostream& out, const SymbolData& sd) {
    // momentan atat
    if (!sd.isFunc()) {
        out << ((sd.isConst() == true) ? "Const variable" : "Variable") << " with type: " << typeToStr(sd.type()) + (sd.type() != CUSTOM ? "" : " " + sd._className) << ", name: " << sd.name() << ", in scope " << sd.scope();
        if (sd.type() == CUSTOM || sd.isArray()) {
            printSubsymbol(out, sd, 1);
        } 
    }
    else {
        out << "Function with type: " << typeToStr(sd._type) << ", name: " << sd.name() << ", in scope " << sd.scope();
    }
    return out;
}

// --- SymbolTable ---

SymbolTable& SymbolTable::add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, const size_t size, const std::string& className) {
    _table.emplace(Scope::scopeWithNameToString(currentScope(), name), std::make_unique<SymbolData>(currentScope(), name, type, flag, size, className));
    //std::cout << "Adding variable: " << Scope::scopeWithNameToString(currentScope(), name) + "\n";
    return *this;
}

SymbolTable& SymbolTable::add(const SymbolData& symbol) {
    _table.emplace(Scope::scopeWithNameToString(currentScope(), symbol.name()), std::make_unique<SymbolData>(symbol));
    //std::cout << "Adding variable: " << Scope::scopeWithNameToString(currentScope(), symbol.name()) + "\n";
    return *this;
}

SymbolTable& SymbolTable::addClass(const std::string& name) {
    _classesTable.emplace(name, std::make_unique<SymbolData>(currentScope(), name, Type::CUSTOM, SymbolData::Flag::Class, 0, name));
    return *this;
}

SymbolTable& SymbolTable::remove(const SymbolData& data) {
    //std::cout << "Removed element " << data.scope() + data.name() << '\n';
    _table.erase(data.scope() + data.name());
    return *this;
}

bool SymbolTable::contains(const std::string& name) {
    return _table.contains(Scope::scopeWithNameToString(currentScope(), name));
}

void SymbolTable::print(std::ostream& out) {
    for (const auto& [path, smb] : _table) {
        out << *smb << '\n';
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
    //std::cout << "Current scope: " << currentScope() << '\n';
}

// todo: specify if i'm entering a class scope or a fxn scope. We might need that info
void SymbolTable::enterScope(const std::string& str) {
    _currentScopeHierarchy.push_back(str);
    //std::cout << "Current scope: " << currentScope() << "\n";
}

void SymbolTable::exitScope() {
    _currentScopeHierarchy.pop_back();
    //std::cout << "Current scope: " << currentScope() << "\n";
}

SymbolData* SymbolTable::find(const std::string& scopedName) {
    auto it = _table.find(scopedName);
    if (it != _table.end()) {
        return it->second.get();
    }
    return nullptr;
}

SymbolData* SymbolTable::findId(const std::string& id) {
    std::string scope = "";
    for (const std::string& scopeAdd: _currentScopeHierarchy) {
        scope += scopeAdd + "/";
        auto it = _table.find(scope + id);
        if (it != _table.end()) {
            return it->second.get();
        }
    }
    return nullptr;
}

SymbolData* SymbolTable::findClass(const std::string& name) {
    auto it = _classesTable.find(name);
    if (it != _classesTable.end()) {
        return it->second.get();
    }
    return nullptr;
}