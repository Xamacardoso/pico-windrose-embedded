#include "temp.h"

float read_temperature(){
    adc_select_input(4); // Seleciona o canal do sensor de temperatura
    uint16_t raw_value = adc_read(); // Lê o valor bruto do ADC
    float voltage = (raw_value * 3.3) / 4095; // Converte para tensão (0-3.3V)
    float temperature = (voltage - 0.5) * 100; // Converte para temperatura em Celsius
    return temperature;
}