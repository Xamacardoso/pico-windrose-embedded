#include "buttons.h"

void buttons_init(){
    // Inicializa os pinos dos botões
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_A); // Habilita pull-up interno
    gpio_pull_up(BUTTON_B);
}