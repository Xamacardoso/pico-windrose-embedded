#ifndef BUTTONS_H
#define BUTTONS_H

#include "pico/stdlib.h"

// Configuração dos botões
#define BUTTON_A 5
#define BUTTON_B 6

// Estrutura para armazenar os dados dos dois botões
typedef struct {
    uint8_t button_a;
    uint8_t button_b;
} button_data_t;

void buttons_init();

#endif