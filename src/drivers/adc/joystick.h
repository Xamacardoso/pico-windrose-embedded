#ifndef JOYSTICK_H
#define JOYSTICK_H
#include "hardware/adc.h"

// Configuração do joystick
#define JOYSTICK_VRX 27
#define JOYSTICK_VRY 26
#define JOYSTICK_MAX_VALUE 99
#define JOYSTICK_MIN_VALUE 0

// Estrutura para armazenar os dados do joystick
typedef struct {
    uint8_t x;
    uint8_t y;
} joystick_data_t;

uint8_t read_joystick_x();
uint8_t read_joystick_y();
void joystick_init();

#endif