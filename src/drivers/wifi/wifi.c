#include "wifi.h"

char request_buffer[256];
struct tcp_pcb* global_pcb = NULL;
bool tcp_busy = false;  // Flag para controlar o estado da conexão

wifi_config_t network_config = {
    .ssid = WIFI_SSID,
    .password = WIFI_PASSWORD,
    .api_host = SERVER_ADDRESS,
    .api_port = SERVER_PORT,
    .api_endpoint = "/update"
};

int wifi_init(wifi_config_t *network_config){
    printf("[WIFI INIT]: Conectando ao wifi...\n");

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("[WIFI INIT]: Falha ao inicializar chip...\n");
        return -1;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("[WIFI INIT]: Falha ao conectar ao WiFi\n");
        return 1;
    } else {
        printf("[WIFI INIT]: Conectado ao WiFi!!!\n");
        
        // Read the ip address in a human readable way
        char *ip_addr = ip4addr_ntoa(netif_ip4_addr(netif_list));
        printf("Endereço IP: %s\n", ip_addr);
    }

    return 0;
}

// Callback para receber dados
err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        // Conexão fechada pelo servidor
        printf("[TCP]: Conexão fechada pelo servidor\n");
        tcp_close(tpcb);
        tcp_busy = false;
        return ERR_OK;
    }

    // Processar dados recebidos (opcional)
    printf("[TCP]: Recebeu %d bytes\n", p->tot_len);
    
    // Libere o buffer recebido
    pbuf_free(p);
    
    return ERR_OK;
}

// Callback quando os dados foram enviados
err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("[TCP]: Dados enviados (%d bytes)\n", len);
    return ERR_OK;
}

// Callback de erro
void tcp_err_callback(void *arg, err_t err) {
    printf("[TCP]: Erro %d ocorreu\n", err);
    global_pcb = NULL;  // Importante: o PCB não é mais válido neste ponto
    tcp_busy = false;
}

// Callback após conexão estabelecida
err_t on_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("[TCP]: Erro ao conectar: %d\n", err);
        tcp_busy = false;
        return err;
    }

    printf("[TCP] Conectado ao servidor!\n");
    
    // Configurar callbacks
    tcp_recv(tpcb, tcp_recv_callback);
    tcp_sent(tpcb, tcp_sent_callback);
    tcp_err(tpcb, tcp_err_callback);
    
    // Enviar dados
    err_t write_err = tcp_write(tpcb, request_buffer, strlen(request_buffer), TCP_WRITE_FLAG_COPY);
    if (write_err != ERR_OK) {
        printf("[TCP]: Erro ao enviar dados: %d\n", write_err);
        tcp_close(tpcb);
        tcp_busy = false;
        return write_err;
    }
    
    tcp_output(tpcb);
    
    // Não fechamos a conexão aqui - esperamos a resposta do servidor
    return ERR_OK;
}

void cleanup_connection(void) {
    if (global_pcb) {
        tcp_close(global_pcb);
        global_pcb = NULL;
    }
    tcp_busy = false;
}

void connect_to_server(const ip_addr_t *ipaddr) {
    // Verificar se já existe uma conexão ativa
    if (tcp_busy) {
        printf("[TCP] Conexão já em andamento, aguarde...\n");
        return;
    }

    // Limpa qualquer PCB anterior
    if (global_pcb) {
        printf("[TCP] Abortando PCB anterior\n");
        tcp_abort(global_pcb);
        global_pcb = NULL;
    }

    global_pcb = tcp_new_ip_type(IPADDR_TYPE_V4);
    if (!global_pcb) {
        printf("[NETWORK]: Erro criando PCB\n");
        return;
    }

    tcp_busy = true;
    err_t err = tcp_connect(global_pcb, ipaddr, SERVER_PORT, on_connected);
    if (err != ERR_OK) {
        printf("[TCP]: Erro ao conectar: %d\n", err);
        tcp_abort(global_pcb);
        global_pcb = NULL;
        tcp_busy = false;
    }
}

// Callback DNS
void dns_callback(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (!ipaddr) {
        printf("[DNS]: Falha ao resolver %s\n", name);
        return;
    }

    tcp_busy = false;

    printf("[DNS]: IP resolvido: %s\n", ipaddr_ntoa(ipaddr));
    connect_to_server(ipaddr);
}

// Envia requisição POST com dados do botão e temperatura
void send_http_post(joystick_data_t joystick, button_data_t buttons, float temp) {
    // Verificar se já existe uma conexão ativa
    if (tcp_busy) {
        printf("[TCP] Sistema ocupado, ignorando requisição\n");
        return;
    }

    char *json_body = createJson(joystick, buttons, temp);

    snprintf(request_buffer, sizeof(request_buffer),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n\r\n"
        "%s",
        network_config.api_endpoint,
        network_config.api_host,
        strlen(json_body),
        json_body
    );

    // Limpando o corpo do json
    free(json_body);

    printf("[NETWORK]: Resolvendo domínio %s...\n", network_config.api_host);
    static ip_addr_t resolved_ip;

    tcp_busy = true;
        
    // Iniciando resolucao DNS
    err_t err = dns_gethostbyname(network_config.api_host, &resolved_ip, dns_callback, NULL);
    
    if (err == ERR_OK) {
        // IP já em cache
        printf("[DNS]: IP já em cache\n");
        tcp_busy = false;
        connect_to_server(&resolved_ip);
    }
    else if (err == ERR_INPROGRESS) {
        printf("[DNS]: Resolução em progresso...\n");
    }
    else {
        printf("[DNS]: Erro ao iniciar resolução DNS: %d\n", err);
        tcp_busy = false;
    }
}

// Função para verificar periodicamente o estado da conexão
void check_connection_timeout(void) {
    static uint32_t last_check_time = 0;
    static uint32_t connection_start_time = 0;
    
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Se estamos ocupados com uma conexão
    if (tcp_busy) {
        // Primeira vez que entramos em estado ocupado?
        if (connection_start_time == 0) {
            connection_start_time = current_time;
        }
        
        // Verificar timeout (0,5 segundos)
        if (current_time - connection_start_time > 500) {
            printf("[TCP]: Timeout da conexão, liberando recursos\n");
            if (global_pcb) {
                tcp_abort(global_pcb);
                global_pcb = NULL;
            }
            tcp_busy = false;
            connection_start_time = 0;
        }
    } else {
        connection_start_time = 0;
    }
    
    last_check_time = current_time;
}

char *createJson(joystick_data_t joystick, button_data_t buttons, float temp) {
    // Criar objeto JSON
    cJSON *data = cJSON_CreateObject();
    
    cJSON_AddBoolToObject(data, "btn_a", buttons.button_a);
    cJSON_AddBoolToObject(data, "btn_b", buttons.button_b);
    cJSON_AddNumberToObject(data, "joystick_x", joystick.x);
    cJSON_AddNumberToObject(data, "joystick_y", joystick.y);
    cJSON_AddNumberToObject(data, "temp", temp);

    char *json_str = cJSON_PrintUnformatted(data);
    cJSON_Delete(data);

    return json_str;
}