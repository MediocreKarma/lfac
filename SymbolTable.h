#include <iostream>
#include <vector>

struct SymbolData {
    // dumnezeu stie
};


class SymbolTable {
    // ma gandesc sa avem o instanta a lu aista care sa managerieze gen
    // scope urile si asa pt basically tot ce avem
    // at its core sa fie efectiv un unordered_map<std::string, SymbolData>, sau ceva la modul... sau depinde daca vrei sa faci de ex ca "x" sa poata avea valori complet diferite in scope-uri diferite
    // ar trebui sa ne gandim apoi cum definim un scope
};