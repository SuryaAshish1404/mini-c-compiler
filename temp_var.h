#ifndef TEMP_VAR_H
#define TEMP_VAR_H

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


void reset_temp_counter();
void get_temp_var(char *buffer, size_t size);
void get_label(char *buffer, size_t size);


void get_loop_var(char *buffer, size_t size, int level);

#ifdef __cplusplus
}
#endif

#endif 
