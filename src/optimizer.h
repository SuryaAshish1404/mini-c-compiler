#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

// Constant folding: evaluate constant expressions at compile time
int optimize_constant_folding(IRList* ir_list);

// Common subexpression elimination: reuse previously computed results
int optimize_common_subexpression_elimination(IRList* ir_list);

// Dead code elimination: remove unused assignments
int optimize_dead_code_elimination(IRList* ir_list);

// Run all optimizations
int optimize_ir(IRList* ir_list);

#ifdef __cplusplus
}
#endif

#endif
