#include "./unitTests.h"
#include "./main.h"

//check if acceleration gets modified on isr call
void test_accel_mod() {
    accel_isr();
    assert(get_accel() == 10);
}

//Test that sample rates are modified 
void test_sample_rate() {
    assert(get_freq_from_temp(0) == 1);
    assert(get_freq_from_temp(26) == 500);
    assert(get_freq_from_temp(30) == 1000);
    assert(get_freq_from_temp(42) == 1000);
    assert(get_freq_from_temp(50) == 2000);
    assert(get_freq_from_temp(100) == 2000);
    assert(get_freq_from_temp(41283) == 2000);
}

// check temperature is read every 1000 and that time resets
void test_temp_read(){
    uint16_t currentTemp = get_temp();
    for(int i=0; i < 1000; i++){
        tick();
        assert(get_temp() == currentTemp);
    }
    tick();
    assert(get_temp()!=currentTemp);
    assert(get_time() == 0);
}

//Test the temperature is sent in the correct conditions
void test_computer_msg_sent(){
    char buffer[100];
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));  // Redirect stdout to buffer
    
    for(int i=0; i < 100; i++){
        read_temp();
        if(get_temp() >= 50){
            fflush(stdout);
            assert(strstr(buffer, "Too Hot") != NULL);
        }
        if(get_temp() <= 10){
            fflush(stdout);
            assert(strstr(buffer, "Too Cold") != NULL);
        }
    }   
}

// Test receiving packets updates values and if required the acceleration/temperature is sent over uart
void test_receive(){
    char buffer[100];
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));  // Redirect stdout to buffer
    
    set_temp(40);

    uint8_t package1[3] = {0, 20, 19};
    set_rx_buff(package1);
    on_packet_received();
    fflush(stdout);
    assert(get_low_thresh() == 10);
    assert(get_high_thresh() == 50);

    uint8_t package2[3] = {1, 80, 12};
    set_rx_buff(package2);
    on_packet_received();
    fflush(stdout);
    assert(strstr(buffer, "Acceleration: 10, Temperature: 40") != NULL);
    assert(get_low_thresh() == 10);
    assert(get_high_thresh() == 50);

    uint8_t package3[3] = {0, 12, 50};
    set_rx_buff(package3);
    on_packet_received();
    fflush(stdout);
    assert(get_low_thresh() == 12);
    assert(get_high_thresh() == 50);

    uint8_t package4[3] = {1, 59, 80};
    set_rx_buff(package4);
    on_packet_received();
    fflush(stdout);
    assert(strstr(buffer, "Acceleration: 10, Temperature: 40") != NULL);
    assert(get_low_thresh() == 59);
    assert(get_high_thresh() == 80);
}

/*
To test the mutexes, I would put breakpoints in critical sections to ensure the mutexes are acquired.
*/