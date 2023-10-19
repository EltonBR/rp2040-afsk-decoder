#include "pico/stdlib.h"
#include<stdio.h>
#include "pico/multicore.h"
#include<cmath>
#include<hardware/adc.h>
#include<inttypes.h>
#include<list>
#define forx(i, sizemax) for(int i = 0; i <= sizemax; i++)

#define SAMPLE_SIZE 10000

typedef struct {
    uint16_t bit0min;
    uint16_t bit0max;    
    uint16_t bit1min;    
    uint16_t bit1max;    
} bits_value_range_t;


void sort(uint16_t*ptr, uint16_t len) {
rpt:
    uint16_t aux;
    bool needsRepeat = false;
    for(uint16_t i = 0; i <= len-1; i++) {
        if (ptr[i] < ptr[i+1]) {
            aux = ptr[i];
            ptr[i] = ptr[i+1];
            ptr[i+1] = aux;
            needsRepeat = true;
        }
    }
    if (needsRepeat) {
        goto rpt;
    }

}


uint64_t tt = time_us_64();
uint64_t lastcallt = time_us_64();
uint64_t lastcallt2 = time_us_64();
void edgeUp(uint16_t val) {    
    if (val == 3000) {
        tt = time_us_64();
    }
}

uint64_t edgeDown(uint16_t val) {
    if(val == 2500) {
        return time_us_64()-tt;
    }
    return 0;
}

uint64_t timers = time_us_64();
void __not_in_flash_func(rx_func)() {
    uint16_t index2 = 0;
    uint16_t index = 0;
    uint16_t buff[SAMPLE_SIZE] = {'\0'} ;
    uint16_t peaks[SAMPLE_SIZE] = {'\0'}; 
    while(1) {

        uint16_t adc_val = adc_read();
        //printf("%i\n", adc_val);

        if (index2 < SAMPLE_SIZE) {
            peaks[index2++] = adc_val;
        }

        if (adc_val < 2500) {
            adc_val = 2500;
        }

        if (adc_val > 3000) {
            adc_val = 3000;
        }

        if (adc_val > 2500) {
            edgeUp(adc_val);
        }
        
        if (adc_val < 3000) {
            buff[index] = edgeDown(adc_val);
            index++;
            uint16_t cc = 0;
            uint16_t prevRead = 0;
            if (index > SAMPLE_SIZE) {
                for(int x = 0; x < SAMPLE_SIZE; x++) {
                        
                    if (buff[x] > 0) { //count a sequence > 0
                        cc++; //counting
                    } else{
                        //done count
                        int bit = 2;
                        if (cc != 0) {
                            if (cc > 30 && cc < 125) {
                                bit = 1;
                            }
                            if (cc > 135 && cc < 170) {
                                bit = 0;
                            }

                            printf("%i\t%i\n",bit, cc);
                            prevRead = cc;
                        }
                        cc=0;
                    }
                }
                index = 0;
                break;
            }
        }

    }

}

int main() {
    //set_sys_clock_khz(125e3, true);
    bool runrx = true; 
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);
    adc_init();
    adc_select_input(2);
    adc_gpio_init(28);
    repeating_timer tt;

    adc_set_clkdiv(0);
    adc_run(false);
    

    while(1) {
    
    if (adc_read() > 2800) {
        rx_func();
    } else{
        //printf("waiting start bit\n");
    }
       /* uint8_t opt = getchar();
        if ( opt == 'c') {

        } else if (opt == 'r') {

        }*/
    }

    return 0;
}
