#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "drivers/adc/joystick.h"
#include "drivers/adc/temp.h"
#include "drivers/gpio/buttons.h"
#include "drivers/wifi/wifi.h"

// Declaração da função de verificação de timeout (que está no wifi.c)
extern void check_connection_timeout(void);

// Controle de taxa de envio
#define SEND_INTERVAL_MS 2000  // Enviar dados a cada 2 segundos
static uint32_t last_send_time = 0;

void setup(){
    stdio_init_all();
    
    // Inicializa joystick e sensor de temperatura
    adc_init();
    joystick_init();
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura
    buttons_init();
    
    // Inicializa WiFi - deixamos por último para dar tempo aos outros periféricos inicializarem
    if (wifi_init(&network_config) != 0) {
        printf("[MAIN]: Erro ao inicializar WiFi, tentando novamente em 5 segundos...\n");
        sleep_ms(5000);
        wifi_init(&network_config);  // Uma segunda tentativa
    }
}

int main()
{
    setup();
    joystick_data_t joystick;
    button_data_t buttons;
    float temp; // Temperatura sensor interno
    
    // Aguardar um pouco após inicialização para estabilizar conexões
    sleep_ms(1000);
    
    while (true) {
        // Sempre verificar o timeout de conexões - isso libera recursos em caso de falha
        check_connection_timeout();
        
        // Ler os sensores
        buttons.button_a = !gpio_get(0); // Lê o botão A
        buttons.button_b = !gpio_get(1); // Lê o botão B
        joystick.x = read_joystick_x();
        joystick.y = read_joystick_y();
        temp = read_temperature(); // Lê a temperatura do sensor interno

        // Exibe os valores lidos
        printf("[MAIN]: Joy X: %d || Joy Y: %d || Btn A: %d ||  Btn B: %d || Temp: %.2f C\n", 
               joystick.x, joystick.y, buttons.button_a, buttons.button_b, temp);
        
        // Controle de taxa de envio de dados
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_send_time >= SEND_INTERVAL_MS) {
            printf("[MAIN]: Enviando dados para o servidor...\n");
            send_http_post(joystick, buttons, temp);
            last_send_time = current_time;
        }
        
        // Pequena pausa para não sobrecarregar a CPU e o console
        sleep_ms(100);  // Intervalo reduzido para manter a responsividade, mas ainda é executado 10x por segundo
    }
}