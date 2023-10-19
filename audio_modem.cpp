#include "pico/stdlib.h"
#include<stdio.h>
#include "pico/multicore.h"
#include<cmath>
#include<hardware/adc.h>
#include<inttypes.h>
#include<list>
#define forx(i, sizemax) for(int i = 0; i <= sizemax; i++)
#define SAMPLE_SIZE 10000
#define DEBUG
typedef struct {
    uint16_t bit0min;
    uint16_t bit0max;    
    uint16_t bit1min;    
    uint16_t bit1max;    
} bits_value_range_t;

void sort(uint16_t*ptr, uint16_t len) {
rpt:
    uint16_t aux;
    bool needs_repeat = false;
    for(uint16_t i = 0; i <= len-1; i++) {
        if (ptr[i] < ptr[i+1]) {
            aux = ptr[i];
            ptr[i] = ptr[i+1];
            ptr[i+1] = aux;
            needs_repeat = true;
        }
    }
    if (needs_repeat) {
        goto rpt;
    }
}

uint64_t wave_timer = time_us_64();

void store_start_time(uint16_t val) {    
    if (val == 3000) {
        wave_timer = time_us_64();
    }
}

uint64_t get_start_end_diff(uint16_t val) {
    if(val == 2500) {
        return time_us_64()-wave_timer;
    }
    return 0;
}

void __not_in_flash_func(rx_func)() {
    uint16_t index = 0;
    uint16_t buff[SAMPLE_SIZE] = {'\0'} ;
    while(1) {

        uint16_t adc_val = adc_read();
        //set min max limits
        if (adc_val < 2500) {
            adc_val = 2500;
        }

        if (adc_val > 3000) {
            adc_val = 3000;
        }

        //detect up start counter
        if (adc_val > 2500) {
            store_start_time(adc_val);
        }
        //detect down stop
        if (adc_val < 3000) {

            //get time diff us and store to proccess data
            buff[index] = get_start_end_diff(adc_val);
            index++;
            uint16_t sequence_counter = 0; //quantity of adc samples per wave cicle

            if (index > SAMPLE_SIZE) {
                for(int x = 0; x < SAMPLE_SIZE; x++) {
                        
                    if (buff[x] > 0) { //count a sequence > 0
                        sequence_counter++; //counting
                    } else{
                        //done count
                        int bit = 2;
                        if (sequence_counter != 0) {
                            if (sequence_counter > 30 && sequence_counter < 125) {
                                bit = 1;
                            }
                            if (sequence_counter > 135 && sequence_counter < 170) {
                                bit = 0;
                            }

                            printf("%i\t%i\n",bit, sequence_counter);

                        }
                        sequence_counter=0;
                    }
                }
                index = 0;
                break;
            }
        }

    }

}

int main() {

    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    adc_init();
    adc_select_input(2);
    adc_gpio_init(28);
    adc_set_clkdiv(0);
    adc_run(false);
    

    while(1) {
        //start rx trigger
        if (adc_read() > 2800) {
            rx_func();
        } else{
            //printf("waiting start bit\n");
        }
    }

    return 0;
}
