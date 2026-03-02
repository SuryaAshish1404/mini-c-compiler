#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

enum class SymbolKind {
    VARIABLE,
    FUNCTION,
    PARAMETER,
    TENSOR
};

struct SymbolEntry {
    std::string name;
    std::string type;
    SymbolKind kind;
    int line_declared;
    int scope_level;
    
    // Tensor-specific fields
    bool is_tensor;
    int num_dimensions;
    std::vector<int> shape;
    
    SymbolEntry() : is_tensor(false), num_dimensions(0) {}
};

class SymbolTable {
public:
    SymbolTable();

    void enter_scope();
    void exit_scope();
    int current_scope() const;

    bool insert(const std::string& name, const std::string& type,
                SymbolKind kind, int line_number);
    
    bool insert_tensor(const std::string& name, const std::vector<int>& shape,
                       int line_number);

    SymbolEntry* lookup(const std::string& name);
    SymbolEntry* lookup_current_scope(const std::string& name);

    void print() const;

private:
    struct Scope {
        std::unordered_map<std::string, SymbolEntry> symbols;
    };

    std::vector<Scope> scopes_;
    int scope_level_;
};

#endif // SYMBOL_TABLE_H
