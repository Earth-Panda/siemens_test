#include "./sensorAPI.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <random>

using namespace std;

bool running = true;

void config_accel(uint16_t frequency, function_pointer isr_func) { /*  */ }

void spi_read_accel(uint16_t *val) { 
    /* some code */ 
    //for testing purposes returns 10m/s
    *val = 10;
}


void spi_read_temp(uint16_t *val) {
    /* some code */ 
    //Generates a random number for testing purposes
    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<> distrib(0, 100);

    *val = (uint16_t)distrib(gen);
}
