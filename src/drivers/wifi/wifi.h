#ifndef WIFI_H
#define WIFI_H

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/cyw43_driver.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"

// #include "../../external/cJSON/cJSON.h"
#include "cJSON.h"

#include "../gpio/buttons.h"
#include "../adc/joystick.h"

// Configuração do servidor (substitua pelo seu SSID real e senha)
#define WIFI_SSID "Xaxau"
#define WIFI_PASSWORD "ponguila"

// IP do servidor e porta (substitua pelo IP real do servidor)
#define SERVER_ADDRESS "http://pico-windrose-backend.onrender.com"

// Variáveis globais para o callback de conexão de rede
extern char request_buffer[256];
extern struct tcp_pcb* global_pcb;
extern bool tcp_busy;

// Estrutura para armazenar as configurações de conexão
typedef struct {
    char *ssid;
    char *password;
    char *api_host;
    char *api_endpoint;
} wifi_config_t;

// Estrutura para armazenar os dados da conexão TCP
typedef struct {
    struct tcp_pcb *pcb;
    char *request;
    uint16_t request_len;
    bool complete;
    bool success;
    wifi_config_t *config; // Adicionado para ter acesso ao config
} tcp_connection_t;

extern wifi_config_t network_config;

int wifi_init(wifi_config_t *network_config);
err_t on_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_err_callback(void *arg, err_t err);
void send_http_post(joystick_data_t joystick, button_data_t buttons, float temp);
void connect_to_server(const ip_addr_t *ipaddr);
void cleanup_connection(void);
void dns_callback(const char *name, const ip_addr_t *ipaddr, void *arg);
char *createJson(joystick_data_t joystick, button_data_t buttons, float temp);
void check_connection_timeout(void);

#endif