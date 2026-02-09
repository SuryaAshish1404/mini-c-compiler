#include "symbol_table.h"
#include <iomanip>

SymbolTable::SymbolTable() : scope_level_(-1) {
    enter_scope(); // global scope
}

void SymbolTable::enter_scope() {
    scope_level_++;
    scopes_.emplace_back();
}

void SymbolTable::exit_scope() {
    if (scope_level_ < 0) {
        std::cerr << "[SymbolTable] Error: cannot exit global scope.\n";
        return;
    }
    scopes_.pop_back();
    scope_level_--;
}

int SymbolTable::current_scope() const {
    return scope_level_;
}

bool SymbolTable::insert(const std::string& name, const std::string& type,
                         SymbolKind kind, int line_number) {
    if (scope_level_ < 0 || scopes_.empty()) {
        std::cerr << "[SymbolTable] Error: no active scope.\n";
        return false;
    }

    auto& current = scopes_.back().symbols;
    if (current.find(name) != current.end()) {
        std::cerr << "[SymbolTable] Error (line " << line_number
                  << "): symbol '" << name
                  << "' already declared in current scope.\n";
        return false;
    }

    current[name] = {name, type, kind, line_number, scope_level_};
    return true;
}

SymbolEntry* SymbolTable::lookup(const std::string& name) {
    for (int i = static_cast<int>(scopes_.size()) - 1; i >= 0; --i) {
        auto it = scopes_[i].symbols.find(name);
        if (it != scopes_[i].symbols.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

SymbolEntry* SymbolTable::lookup_current_scope(const std::string& name) {
    if (scopes_.empty()) return nullptr;
    auto it = scopes_.back().symbols.find(name);
    if (it != scopes_.back().symbols.end()) {
        return &it->second;
    }
    return nullptr;
}

void SymbolTable::print() const {
    std::cout << "\n===== Symbol Table =====\n";
    std::cout << std::left
              << std::setw(15) << "Name"
              << std::setw(10) << "Type"
              << std::setw(12) << "Kind"
              << std::setw(8)  << "Line"
              << std::setw(8)  << "Scope"
              << "\n";
    std::cout << std::string(53, '-') << "\n";

    for (size_t i = 0; i < scopes_.size(); ++i) {
        for (const auto& pair : scopes_[i].symbols) {
            const auto& sym = pair.second;
            std::string kind_str;
            switch (sym.kind) {
                case SymbolKind::VARIABLE:  kind_str = "variable";  break;
                case SymbolKind::FUNCTION:  kind_str = "function";  break;
                case SymbolKind::PARAMETER: kind_str = "parameter"; break;
            }
            std::cout << std::left
                      << std::setw(15) << sym.name
                      << std::setw(10) << sym.type
                      << std::setw(12) << kind_str
                      << std::setw(8)  << sym.line_declared
                      << std::setw(8)  << sym.scope_level
                      << "\n";
        }
    }
    std::cout << "========================\n\n";
}
