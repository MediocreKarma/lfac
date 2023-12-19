#include "types.h"

Type::Type(const size_t arr_size) : 
    scope(), is_init(false), is_const(false), array_size(arr_size) {}

BuiltInType::BuiltInType(const std::variant<int, float, char, bool, std::string>& _var, const size_t arr_size) :
    Type(arr_size) {}