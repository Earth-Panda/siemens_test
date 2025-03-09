#include <stdint.h>
typedef void (*function_pointer)();

void config_accel(uint16_t frequency, function_pointer isr_func);

void spi_read_accel(uint16_t *val);

void spi_read_temp(uint16_t *val);

void hardware_accel(function_pointer callback);

void hardware_tick(function_pointer callback);
