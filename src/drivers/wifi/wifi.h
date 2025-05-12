#ifndef WIFI_H
#define WIFI_H

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/cyw43_driver.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"

#include "../../external/cJSON/cJSON.h"

#include "../gpio/buttons.h"
#include "../adc/joystick.h"

// Configuração do servidor (substitua pelo seu SSID real e senha)
#define WIFI_SSSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// IP do servidor e porta (substitua pelo IP real do servidor)
#define SERVER_ADDRESS "https://pico-windrose-backend.onrender.com"

// Variáveis globais para o callback de conexão de rede
extern char request_buffer[256];
extern struct tcp_pcb* global_pcb;

int wifi_init();
err_t on_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
void send_http_post(joystick_data_t joystick, button_data_t buttons, float temp);

#endif