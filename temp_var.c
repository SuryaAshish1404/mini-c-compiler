#include "temp_var.h"

static int temp_counter = 0;
static int label_counter = 0;

// Reset temporary variable counter
void reset_temp_counter() {
    temp_counter = 0;
    label_counter = 0;
}

// Generate a new temporary variable name
void get_temp_var(char *buffer, size_t size) {
    snprintf(buffer, size, "t%d", temp_counter++);
}

// Generate a new label name
void get_label(char *buffer, size_t size) {
    snprintf(buffer, size, "L%d", label_counter++);
}

// Generate loop variable for tensor operations
void get_loop_var(char *buffer, size_t size, int level) {
    snprintf(buffer, size, "i%d", level);
}
