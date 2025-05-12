#include "joystick.h"

uint8_t read_joystick_x(){
    adc_select_input(1);

    return adc_read() * 100 / 4095;
}

uint8_t read_joystick_y(){
    adc_select_input(0);

    return adc_read() * 100 / 4095;
}

void joystick_init(){
    adc_gpio_init(JOYSTICK_VRX);
    adc_gpio_init(JOYSTICK_VRY);
}