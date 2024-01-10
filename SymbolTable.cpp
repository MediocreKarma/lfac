#include "SymbolTable.h"
#include "AST.h"

using namespace TypeNms;

std::string fullMatrixForm(const std::vector<size_t> sizes) {
    std::string output = "";
    for (size_t size : sizes) {
        output += "[" + std::to_string(size) + "]";
    }
    return output;
}

std::string Scope::scopeToString(const std::vector<std::string>& scope, ssize_t size) {
    const char* const delim = "/";
    std::string result;
    if (size < 0) {
        for (const auto& scopeStr : scope) {
            result += scopeStr + delim;
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            result += scope[i] + delim;
        }
    }
    return result;
}

std::string Scope::scopeWithNameToString(const std::string& scope, const std::string& name) {
    return scope + name;
}

bool Scope::encompassingScope(const std::string& active, const std::string& encompassing) {
    return active.find(encompassing) != std::string::npos;
}

SymbolData::SymbolData(const std::string& scope, const std::string& name, const TypeNms::Type t, const Flag f, const std::vector<size_t>& sizes, const SymbolData* classDef) {
    _scope = scope;
    _name = name;
    _type = t;
    _sizes = sizes;
    _isArray = (sizes.empty() == false);
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
        case InitList:
            _isInitList = true;
            _isInit = true;
            break;
    }
    if (_isArray == false and (t == CUSTOM or _isFunc or _isClassDef)) {
        _value = std::vector<SymbolData>();
    }
    if (_isArray) {
        if (t != CUSTOM || sizes.size() > 1) {
            std::vector<SymbolData> vec;
            vec.reserve(sizes.front());
            for (size_t i = 0; i < sizes.front(); ++i) {
                vec.emplace_back(scope, name + "[" + std::to_string(i) + "]", t, f, std::vector<size_t>(sizes.begin() + 1, sizes.end()), classDef);
            }
            _value = vec;
        }
        else {
            std::vector<SymbolData> vec;
            vec.reserve(sizes.front());
            for (size_t i = 0; i < sizes.front(); ++i) {
                vec.emplace_back(classDef->instantiateClass(scope, name + "[" + std::to_string(i) + "]"));
            }
            _value = vec;
        }

    }
    if (t == CUSTOM && _isInitList == false) {
        if (classDef == nullptr) {
            if (_isClassDef) {
                _className = name;
            }
            else {
                throw std::logic_error("Custom type constructed without any class-instance template");
            }
        }
        else {
            _className = classDef->name();
        }
    }
}

SymbolData::SymbolData(const std::string& scope, const std::string& name, const TypeNms::Type t, const Flag f, const Value& val) :
    SymbolData(scope, name, t, f) {
    _value = val;
}

SymbolData& SymbolData::setConst(bool value) {
    if (_isConst and value == false) {
        throw std::logic_error("Cannot make const symbol " + _name + " non-const");
    }
    if (value && _isInit == false) {
        throw std::runtime_error("Cannot initialize const variable " + name() + " because it is not fully initialized");
    }
    _isConst = value;
    if (_isConst && std::holds_alternative<std::vector<SymbolData>>(_value)) {
        for (SymbolData& subSymbol : std::get<std::vector<SymbolData>>(_value)) {
            try {
                subSymbol.setConst(value);
            }
            catch(std::runtime_error& e) {
                throw std::runtime_error("Cannot initialize const variable " + name() + " because it is not fully initialized");
            }
        }
    }
    return *this;
}

SymbolData& SymbolData::setType(TypeNms::Type type) {
    _type = type;
    return *this;
}

SymbolData& SymbolData::assign(const SymbolData& symbol) {
    if (std::holds_alternative<std::vector<SymbolData>>(value()) == false) {
        this->assign(symbol.value());
        return *this;
    }
    //throwWhenUnassignable(symbol.value());
          std::vector<SymbolData>&   thisVec = std::get<std::vector<SymbolData>>(_value);
    const std::vector<SymbolData>& symbolVec = std::get<std::vector<SymbolData>>(symbol._value);
    std::string from  = TypeNms::typeToStr(symbol.type());
    std::string to    = TypeNms::typeToStr(type());
    if (type() == CUSTOM) {
        to += ' ' + className();
    }
    if (symbol.type() == CUSTOM) {
        from += ' ' + symbol.className();
    }
    if (isArray()) {
        from += fullMatrixForm(_sizes);
    }
    if (symbol.isArray()) {
        to += fullMatrixForm(symbol._sizes);
    }
    if (sameType(*this, symbol) == false || thisVec.size() != symbolVec.size()) {
        throw std::runtime_error("Cannot assign type: " + from + " to type: " + to);
    } 
    _isInit = true;
    for (size_t i = 0; i < thisVec.size(); ++i) {
        thisVec[i].assign(symbolVec[i]);
    }
    return *this;
}

SymbolData& SymbolData::assign(const Value& val) {
    using namespace TypeNms;
    throwWhenUnassignable(val);
    _isInit = true;
    if (_type != CUSTOM and !_isArray) { 
        _value = val;
        return *this;
    }

    if (!std::holds_alternative<std::vector<SymbolData>>(_value)) {
        throw std::logic_error("Cannot iterate through array or non-base-type symbol " + _name + " in order to assign to it. Something has gone very wrong");
    }

    auto& vector = std::get<std::vector<SymbolData>>(_value);
    auto& valueSymbols = std::get<std::vector<SymbolData>>(val);
    for (size_t i = 0; i < std::min(vector.size(), valueSymbols.size()); ++i) {
        vector[i].assign(valueSymbols[i].value());
    }

    return *this;
}

void SymbolData::throwWhenUnassignable(const Value& val) {
    if (isConst()) {
        throw std::runtime_error("Cannot assign to constant symbol " + _name);
    }
    if (isFunc()) {
        throw std::runtime_error("Cannot assign to function symbol " + _name);
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
    std::cout << *this << '\n';
    const std::vector<SymbolData>&  thisData = std::get<std::vector<SymbolData>>(value());
    const std::vector<SymbolData>& otherData = std::get<std::vector<SymbolData>>(val);
    if (thisData.size() < otherData.size()) {
        throw std::runtime_error("Initializer list has too many parameters");
    }
    else if (thisData.size() > otherData.size()) {
        Utils::printWarning("Initializer list has too few parameters for symbol" + ((_name.empty()) ? "" : " " + _name) + ". Part of symbol will remain uninitialized");
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

SymbolData& SymbolData::addSymbolToBeginning(const SymbolData& symbol) {
    if (_isArray or !std::holds_alternative<std::vector<SymbolData>>(_value)) {
        throw std::runtime_error("Cannot add symbol to symbol " + _name + " that does not hold other symbols");
    }
    std::vector<SymbolData>& vec = std::get<std::vector<SymbolData>>(_value);
    vec.insert(vec.begin(), symbol);
    return *this;
}


SymbolData& SymbolData::addSymbol(const SymbolData& symbol) {
    if (_isArray or !std::holds_alternative<std::vector<SymbolData>>(_value)) {
        throw std::runtime_error("Cannot add symbol to symbol " + _name + " that does not hold other symbols");
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

const std::vector<size_t>& SymbolData::sizes() const {
    return _sizes;
}

void SymbolData::recursiveScopeApply() {
    if (std::holds_alternative<std::vector<SymbolData>>(value())) {
    for (SymbolData& subSymbol : std::get<std::vector<SymbolData>>(_value)) {
            subSymbol._scope = _scope;
            subSymbol.recursiveScopeApply();
        }
    }
}

SymbolData SymbolData::instantiateClass(const std::string& scope, const std::string& name) const {
    if (!_isClassDef) {
        throw std::invalid_argument("Cannot clone non-class-template symbol " + _name);
    }
    SymbolData symbol(*this);
    symbol._scope = scope;
    symbol.recursiveScopeApply();
    symbol._name = name;
    symbol._className = _className;
    return symbol;
}

SymbolData* SymbolData::member(const std::string& id) {
    if (_isArray || _isFunc) {
        return nullptr;
    }
    if (_type != TypeNms::Type::CUSTOM) {
        return nullptr;
    }
    if (std::holds_alternative<std::vector<SymbolData>>(value()) == false) {
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
        throw std::invalid_argument("Cannot apply index operator to non-array symbol");
    }
    std::vector<SymbolData>* vec = std::get_if<std::vector<SymbolData>>(&_value);
    if (index >= vec->size()) {
        return nullptr;
    }
    return &(*vec)[index];
}

bool SymbolData::hasSameTypeAs(const SymbolData& sym) const {
    return sameType(*this, sym);
}

bool sameType(const SymbolData& a, const SymbolData& b) {
    if (b._isInitList == false || std::holds_alternative<std::vector<SymbolData>>(a.value()) == false) {
        if (a.type() != CUSTOM and !a.isFunc()) {
            return a.type() == b.type(); 
        }
        if (a.type() != b.type()) {
            return false;
        }
        if (a.type() == CUSTOM and b.type() == CUSTOM 
            and !a.isFunc() and !b.isFunc()
            and !a.isArray() and !b.isArray()
            and  a.className() != b.className()) {
            return false;
        }
    }
    const auto& aData = std::get<std::vector<SymbolData>>(a.value());
    const auto& bData = std::get<std::vector<SymbolData>>(b.value());
    if (b._isInitList == false) {
        if (aData.size() != bData.size()) {
            return false;
        }
        if (a.isArray() != b.isArray() or a.isFunc() != b.isFunc()) {
            return false;
        }
    }
    for (size_t i = 0; i < aData.size(); ++i) {
        if (!sameType(aData[i], bData[i])) {
            return false;
        }
    }
    return true;
}

void printSubsymbol(std::ostream& out, const SymbolData& sd, size_t depth) {
    if (depth > 0) {
        out << '\n' << std::string(depth, '\t');
    }
    if (!sd.isFunc()) {
        out << (sd.isConst() ? "Const variable" : "Variable") << " with type: " << typeToStr(sd.type()) + (sd.type() != CUSTOM ? "" : " " + sd._className) 
            << (sd.isArray() ? fullMatrixForm(sd.sizes()) : "") << ", name: \'" + sd.name() + "\'" << (depth == 0 ? ", in scope \'" + sd.scope() + '\'' : "");
        
        
        if (sd.type() == CUSTOM or sd.isArray()) {
            for (const auto& subSymbol : std::get<std::vector<SymbolData>>(sd.value())) {
                printSubsymbol(out, subSymbol, depth + 1);
            }
        }
        else {
            const auto val = sd.value();
            out << ", with value " << sd.valueStr();
        }
    }
    else {
        out << "Function with type: " << typeToStr(sd._type) + (sd.type() != CUSTOM ? "" : " " + sd._className) << ", name: \'" + sd.name() + "\'" << (depth == 0 ? ", in scope \'" + sd.scope() + '\'' : "");
        for (const auto& subSymbol : std::get<std::vector<SymbolData>>(sd.value())) {
            printSubsymbol(out, subSymbol, depth + 1);
        }
    }
}

std::ostream& operator << (std::ostream& out, const SymbolData& sd) {
    printSubsymbol(out, sd, 0);
    return out;
}


std::string SymbolData::valueStr() const {
    using namespace TypeNms;
    if (!_isInit) {
        return "uninitialized";
    }
    switch(type()) {
        case INT :
            return std::to_string(std::get<int>(_value));
        case FLOAT:
            return std::to_string(std::get<float>(_value));
        case BOOL:
            return (std::get<bool>(_value) == 1) ? "true" : "false";
        case CHAR:
            return "\'" + Utils::encodeStringValue(std::string() + std::get<char>(_value)) + "\'";
        case STRING:
            return "\"" + Utils::encodeStringValue(std::get<std::string>(_value)) + "\"";
        default:
            return "non-base-type";
    }
}

std::string SymbolData::trueValueStr() const {
    
}

// --- SymbolTable ---

SymbolTable& SymbolTable::add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, const std::vector<size_t>& sizes) {
    _table.emplace(Scope::scopeWithNameToString(currentScope(), name), std::make_unique<SymbolData>(currentScope(), name, type, flag, sizes));
    _orderedTable.push_back(_table.at(Scope::scopeWithNameToString(currentScope(), name)).get());
    return *this;
}

SymbolTable& SymbolTable::add(const std::string& name, TypeNms::Type type, SymbolData::Flag flag, const std::vector<size_t>& sizes, const SymbolData* className) {
    _table.emplace(Scope::scopeWithNameToString(currentScope(), name), std::make_unique<SymbolData>(currentScope(), name, type, flag, sizes, className));
    _orderedTable.push_back(_table.at(Scope::scopeWithNameToString(currentScope(), name)).get());
    return *this;
}

SymbolTable& SymbolTable::add(const SymbolData& symbol) {
    _table.emplace(Scope::scopeWithNameToString(currentScope(), symbol.name()), std::make_unique<SymbolData>(symbol));
    _orderedTable.push_back(_table.at(Scope::scopeWithNameToString(currentScope(), symbol.name())).get());
    //std::cout << "Adding variable: " << Scope::scopeWithNameToString(currentScope(), symbol.name()) + "\n";
    return *this;
}

SymbolTable& SymbolTable::addClass(const std::string& name) {
    _classesTable.emplace(name, std::make_unique<SymbolData>(currentScope(), name, Type::CUSTOM, SymbolData::Flag::Class));
    _orderedClassesTable.push_back(_classesTable.at(name).get());
    return *this;
}

SymbolTable& SymbolTable::remove(const SymbolData& data) {
    //std::cout << "Removed element " << data.scope() + data.name() << '\n';
    auto it = _table.find(data.scope() + data.name());
    if (it == _table.end()) {
        return *this;
    }
    for (auto ordIt = _orderedTable.begin(); ordIt != _orderedTable.end(); ++ordIt) {
        if (*ordIt == it->second.get()) {
            _orderedTable.erase(ordIt);
            break;
        }
    }
    _table.erase(data.scope() + data.name());
    return *this;
}

SymbolTable& SymbolTable::setReturnType(TypeNms::Type t, const std::string& className) {
    _returnType = t;
    _returnClass = className;
    return *this;
}

bool SymbolTable::contains(const std::string& name) {
    return _table.contains(Scope::scopeWithNameToString(currentScope(), name));
}

bool SymbolTable::sameReturnType(TypeNms::Type t, const std::string& className) {
    return _returnType == t && _returnClass == className;
}

void SymbolTable::print(std::ostream& out) {
    out << "\n--- CLASS TABLE ---\n" << std::endl;
    for (const auto smb : _orderedClassesTable) {
        out << *smb << '\n';
    }
    out << "\n--- SYMBOL TABLE ---\n" << std::endl;
    for (const auto smb : _orderedTable) {
        out << *smb << '\n';
    }
}

std::string SymbolTable::currentScope() {
    return Scope::scopeToString(_currentScopeHierarchy);
}

void SymbolTable::enterAnonymousScope() {
    static size_t i = 0;
    if (i == SIZE_MAX) {
        throw std::runtime_error("Reached anon scope limit for program");
    }
    std::string scope = std::to_string(i++);
    _currentScopeHierarchy.push_back(scope);
}

void SymbolTable::enterScope(const std::string& str) {
    _currentScopeHierarchy.push_back(str);
}

void SymbolTable::exitScope() {
    _currentScopeHierarchy.pop_back();
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
    for (int i = _currentScopeHierarchy.size(); i >= 0; --i) {
        std::string joinedScope = Scope::scopeToString(_currentScopeHierarchy, i);
        auto it = _table.find(joinedScope + id);
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