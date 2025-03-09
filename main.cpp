#include "./main.h"
#include "./sensorAPI.h"
#include "./uart.h"

// For testing purposes only
#include "./unitTests.h"
using namespace std;

//Global variable to store time
uint32_t g_time = 0;

//Variables are being written/read from, to avoid issues, only modify in critical sections
uint16_t accel_reading = 0;
uint16_t temp_reading = 888;

//RX storage 
uint8_t rx_buff[3];

// Temperature thresholds to avoid issues, only modify in critical sections
uint8_t temp_thresh_low = 10;
uint8_t temp_thresh_high = 50;

// For unit tests
uint16_t get_accel(){
    return accel_reading;
}
uint16_t get_temp(){
    return temp_reading;
}
void set_temp(uint16_t val){
    temp_reading = val;
}
uint32_t get_time(){
    return g_time;
}
uint8_t get_low_thresh(){
    return temp_thresh_low;
}
uint8_t get_high_thresh(){
    return temp_thresh_high;
}
void set_rx_buff(uint8_t* package){
    rx_buff[0] = package[0];
    rx_buff[1] = package[1];
    rx_buff[2] = package[2];
}

// Accelerometer interrupt callback
void accel_isr(){
    //Accessing critical info, acquire mutex for acceleration
    enter_critical_section(ACCEL_MUT);
    spi_read_accel(&accel_reading);
    exit_critical_section(ACCEL_MUT);
}

// Returns a frequency to read from the accelerometer based on the temperature
uint16_t get_freq_from_temp(uint16_t temp) {
    //Note: not specified in the document but a temp of 30 is lumped into the 1kHz range to make it continuous
    if(temp > 0 && temp < 30 ){
        return 500;
    } else if (temp >= 30 && temp < 50){
        return 1000;
    } else if (temp >= 50){
        return 2000;
    } else { //Basically if temp is 0 or lower (invalid)
        return 1; // one hertz to avoid 0 frequency 
    }
}

void read_temp(){
    char tx_buf[40]; 
    int uart_buf_len = 0;
    //Accessing critical info, acquire mutex for temperature
    enter_critical_section(TEMP_MUT);
    spi_read_temp(&temp_reading); // Temp value is assumed to be just represented by a uint16_t and does not need further processing
    uint16_t freq = get_freq_from_temp(temp_reading);
    // Dynamic message to send to computer
    if(temp_reading <= temp_thresh_low){
        uart_buf_len = sprintf(tx_buf, "Too Cold");
    }
    if (temp_reading >= temp_thresh_high){
        uart_buf_len = sprintf(tx_buf, "Too Hot");
    }
    exit_critical_section(TEMP_MUT);

    // This assumes that setting the frequency the same as it was previously has no adverse effects on the system
    config_accel(freq, accel_isr);

    //Once all important operations are done send message over UART
    if(uart_buf_len > 0){
        uart_tx((uint8_t *)tx_buf, uart_buf_len, 1000);
    }
}

// RTC interrupt callback polled every 1 ms
void tick(){
    g_time += 1;
    // every 1s read temperature
    if (g_time > 1000) {
        g_time = 0;
        read_temp(); 
    }
}

//By "on demand" I Assume that the computer sends a request for data fia UART and the MCU responds with the data
void on_packet_received(){
    if(rx_buff[0] == 1){
        //Enter critical section to only copy the current data, nothing else since UART transmission takes a while
        enter_critical_section(TEMP_MUT);
        enter_critical_section(ACCEL_MUT);
        uint16_t temp_copy = temp_reading;
        uint16_t accel_copy = accel_reading; 
        exit_critical_section(TEMP_MUT);
        exit_critical_section(ACCEL_MUT);

        //Bundle the data into a buffer and send it
        char tx_buf[40]; 
		int uart_buf_len=0;
        uart_buf_len = sprintf(tx_buf, "Acceleration: %d, Temperature: %d",accel_copy, temp_copy);
        uart_tx((uint8_t *)tx_buf, uart_buf_len, 1000);
    }

    // Don't modify thresholds if the lower threshold is higher then the higher or if they are equal
    enter_critical_section(TEMP_MUT);
    if(rx_buff[2] > rx_buff[1]){
        temp_thresh_low = rx_buff[1];
        temp_thresh_high = rx_buff[2];
    }
    exit_critical_section(TEMP_MUT);
}


int main() {
    printf("Program Start");

    /* Other initializations... */
    //I assume some sort of UART init fuction would be called to set the UART settings in addition to other initializations
    UART_init();
    /* Other initializations... */

    //initialize the RX channel assume that the UART RX sends one byte with a value of 1 when it wants to read data
    uart_rx(rx_buff, 3, on_packet_received);

    /* Call my unit tests here */
    test_accel_mod();
    test_sample_rate();
    test_temp_read();
    test_computer_msg_sent();
    test_receive();

    /* rest of code includeing main loop...*/
}

