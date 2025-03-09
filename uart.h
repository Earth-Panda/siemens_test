#include <stdint.h>
#include <stdio.h>

#define ACCEL_MUT 0
#define TEMP_MUT 1

//I'm assuming no UART handle is needed (only one available)

// Non blocking rx. Waits to receive packet from UART. I'm assuming it triggers a callback function when it gets called
// I'm passing it as a parameter to be explicit, but on STM (which i'm most familiar with) it's predefined based on some IOC settings
void uart_rx(uint8_t* rxbuff, uint16_t buff_size, void (*callback)());

// This function sends tx buff over UART. timeout is in milliseconds
void uart_tx(uint8_t* txbuff, uint16_t buff_size, uint16_t timeout); 

void UART_init();

//I assume these work like a mutex 
//while only one is provided 2 would be better, one for the acceleration and one for the temperature
//In the even there is only one, they could all be replaced with one with no parameters.
// The mutex_num parameter specifies which mutex is being locked/unlocked.
void enter_critical_section(int mutex_num); // acquire a mutex
void exit_critical_section(int mutex_num); // release mutex

