#include<hardware/pwm.h>
#include<hardware/gpio.h>

void pwm_start() {


     gpio_set_function(25, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(25);
    int slc = 65353;
 
    pwm_set_wrap(slice_num, slc);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, slc/2);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, slc/2);
    pwm_set_enabled(slice_num, true);
}