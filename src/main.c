#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "drivers/adc/joystick.h"
#include "drivers/adc/temp.h"
#include "drivers/gpio/buttons.h"
#include "drivers/wifi/wifi.h"

void setup(){
    stdio_init_all();
    wifi_init();

    // Inicializa joystick e sensor de temperatura
    adc_init();
    joystick_init();
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura
    buttons_init();
}

int main()
{
    setup();
    joystick_data_t joystick;
    button_data_t buttons;
    float temp; // Temperatura sensor interno

    while (true) {
        buttons.button_a = !gpio_get(0); // Lê o botão A
        buttons.button_b = !gpio_get(1); // Lê o botão B
        joystick.x = read_joystick_x();
        joystick.y = read_joystick_y();
        temp = read_temperature(); // Lê a temperatura do sensor interno

        printf("[MAIN]: Joy X: %d || Joy Y: %d || Btn A: %d ||  Btn B: %d || Temp: %.2f C\n", joystick.x, joystick.y, buttons.button_a, buttons.button_b, temp);
        send_http_post(joystick, buttons, temp);
        sleep_ms(500);
    }
}
