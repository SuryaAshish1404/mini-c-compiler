#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"

#ifdef __cplusplus
extern "C" {
#endif

int optimize_constant_folding(IRList* ir_list);
int optimize_common_subexpression_elimination(IRList* ir_list);
int optimize_dead_code_elimination(IRList* ir_list);
int optimize_ir(IRList* ir_list);

#ifdef __cplusplus
}
#endif

#endif
