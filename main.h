#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

uint16_t get_accel();
uint16_t get_temp();
void set_temp(uint16_t val);
uint32_t get_time();
uint8_t get_low_thresh();
uint8_t get_high_thresh();
void set_rx_buff(uint8_t* package);

void accel_isr();

uint16_t get_freq_from_temp(uint16_t temp);

void read_temp();

void tick();

void on_packet_received();