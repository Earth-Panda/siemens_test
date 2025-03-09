#include "./uart.h"

void uart_rx(uint8_t* rxbuff, uint16_t buff_size, void (*callback)()){ /*Assume some code */ }

void uart_tx(uint8_t* txbuff, uint16_t buff_size, uint16_t timeout){ 
    /*Assume some code */ 
    //Test code
    printf((char*)txbuff);
}

void UART_init(){/*some code*/}

void enter_critical_section(int mutex_num){

} 

void exit_critical_section(int mutex_num){

}