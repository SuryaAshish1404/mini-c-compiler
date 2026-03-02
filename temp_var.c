#include "temp_var.h"

static int temp_counter = 0;
static int label_counter = 0;

void reset_temp_counter() {
    temp_counter = 0;
    label_counter = 0;
}

void get_temp_var(char *buffer, size_t size) {
    snprintf(buffer, size, "t%d", temp_counter++);
}

void get_label(char *buffer, size_t size) {
    snprintf(buffer, size, "L%d", label_counter++);
}

void get_loop_var(char *buffer, size_t size, int level) {
    snprintf(buffer, size, "i%d", level);
}
