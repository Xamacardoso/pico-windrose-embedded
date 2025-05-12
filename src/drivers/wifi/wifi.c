#include "wifi.h"

char request_buffer[256];
struct tcp_pcb* global_pcb = NULL;

int wifi_init(){
    printf("[WIFI INIT]: Conectando ao wifi...\n");

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("[WIFI INIT]: Falha ao conectar ao WiFi...\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("[WIFI INIT]: Falha ao conectar ao WiFi\n");
        return 1;
    } else {

        printf("[WIFI INIT]: Conectado ao WiFi!!!\n");
        
        // Read the ip address in a human readable way
        char *ip_addr = ip4addr_ntoa(netif_ip4_addr(netif_list));
        printf("Endereço IP: %s", ip_addr);
    }

    return 0;
}

// Callback após conexão estabelecida
err_t on_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) return err;

    printf("Conectado ao servidor!\n");

    tcp_write(tpcb, request_buffer, strlen(request_buffer), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_close(tpcb); // Fecha a conexão após envio

    return ERR_OK;
}

// Envia requisição POST com dados do botão e temperatura
void send_http_post(joystick_data_t joystick, button_data_t buttons, float temp) {
    uint8_t joy_x = joystick.x;
    uint8_t joy_y = joystick.y;
    uint8_t btn_a = buttons.button_a;
    uint8_t btn_b = buttons.button_b;

    ip4_addr_t dest_ip;
    if (!ip4addr_aton(SERVER_ADDRESS, &dest_ip)) {
        printf("Erro no IP\n");
        return;
    }

    snprintf(request_buffer, sizeof(request_buffer),
        "POST /update?joy_x=%d&joy_y=%d&btn_a=%d&btn_b=%d&temp=%.2f HTTP/1.0\r\nHost: %s\r\n\r\n",
        joy_x, joy_y, btn_a, btn_b, temp, SERVER_ADDRESS);

    global_pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!global_pcb) {
        printf("Erro criando pcb\n");
        return;
    }

    // Conecta ao servidor e define o callback de conexão
    // err_t err = tcp_connect(global_pcb, &dest_ip, SERVER_PORT, on_connected);
    // if (err != ERR_OK) {
    //     printf("Erro ao conectar: %d\n", err);
    //     tcp_abort(global_pcb);
    // }
}