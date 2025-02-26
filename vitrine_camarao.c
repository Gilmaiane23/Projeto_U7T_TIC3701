/*Projeto Desenvolvido por: Gilmaiane Porto Silva*/
/*Vitrine do Camarão*/
/*Projeto que controla mostra a temperatura através do display e 
caso a temperatura esteja fora da faixa aceitável será recomendado
adição de água (temperatura abaixo do nível aceitável), ou adição de
gelo (temperatura acima do nível aceitável)
O sensor foi substituído através da Comunição Serial.*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "buzzer.h"
#include "hardware/pwm.h" // Biblioteca para controlar o hardware de PWM


// Arquivo display
#include "ssd1306.h"
#include "font.h"
#include "hardware/i2c.h"

// Definições de pinos e I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

// Definição dos LEDs
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12

//Controle dos leds
volatile bool led_aceso = true;
//Definição do buzzer
#define BUZZER_PIN 10 // Pino do buzzer conectado à GPIO como PWM
#define FREQUENCY 500


// Botões
const uint PIN_BOTAO_A = 5;  // Botão A
const uint PIN_BOTAO_B = 6;  // Botão B

// Estado dos botões
volatile bool botao_A_pressionado = false;
volatile bool botao_B_pressionado = false;
volatile bool alarme_ativo = false; // Variável para controlar o alarme
volatile bool botao_pressionado = false; // Variável para capturar os botões

// Variáveis para controle do tempo (debounce)
uint32_t last_time_A = 0;
uint32_t last_time_B = 0;

// Inicializa o display
ssd1306_t ssd;

// Protótipo das funções 
void init_display();
void inicializacao();
void gpio_irq_handler(uint gpio, uint32_t events);
void exibir_temperatura(float temperatura);
void verificar_temperatura(float temperatura);
void pisca_led();


int main() {
    // Inicializações
    init_display();
    inicializacao();
    stdio_init_all();   


            while (1) {
                if (stdio_usb_connected()) { // USB conectado, processa comandos
                int c = getchar_timeout_us(1000); // Timeout de 1ms
        
                if (c != PICO_ERROR_TIMEOUT) { 
                    printf("Recebido: '%c'\n", c);

                if (c >= '0' && c <= '9') {
                float temperatura = atof((char[]){c, '\0'}); // Converte para float

                if (temperatura >= 0 && temperatura <= 4) {
                    exibir_temperatura(temperatura);
                } else if (temperatura > 4) { //Temperatura acima de 4°C, ativa alarme chama função do display
                    verificar_temperatura(temperatura);
                    alarme_ativo = true;
                    start_buzzer(BUZZER_PIN, 500);
                    sleep_ms(20);
                    pisca_led();

                } else if (temperatura < 0) { //Temperatura abaixo de 0°C, ativa alarme chama função do display
                    verificar_temperatura(temperatura);
                    alarme_ativo = true;
                    start_buzzer(BUZZER_PIN, 300);
                    sleep_ms(20);
                    pisca_led();
                }
                } else {
                printf("Comando inválido: '%c'\n", c);
            }
        }
    }

    // Verifica se o botão foi pressionado e desliga o alarme
         if (alarme_ativo) {
            start_buzzer(BUZZER_PIN, 500);
        } else {
        stop_buzzer(BUZZER_PIN);
        }

    sleep_ms(100); // Pequeno delay para evitar loop excessivo
    }
}

void init_display() { //Inicializações do display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    sleep_ms(2000);
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0);
    ssd1306_draw_string(&ssd, "Bem Vindo", 30, 10);
    ssd1306_draw_string(&ssd, "Vitrine", 36, 30);
    ssd1306_draw_string(&ssd, "do Camarao!", 30, 48);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
}

void inicializacao() { //Inicializações gerais
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    gpio_init(PIN_BOTAO_A);
    gpio_set_dir(PIN_BOTAO_A, GPIO_IN);
    gpio_pull_up(PIN_BOTAO_A);
    gpio_init(PIN_BOTAO_B);
    gpio_set_dir(PIN_BOTAO_B, GPIO_IN);
    gpio_pull_up(PIN_BOTAO_B);

    adc_init();
    
    gpio_set_irq_enabled_with_callback(PIN_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

void pisca_led() { //Pisca LED
    
        gpio_put(LED_RED, 1); // Liga o LED
        sleep_ms(1000);       // Espera 1 segundo
        gpio_put(LED_RED, 0); // Desliga o LED
        sleep_ms(1000);       // Espera 1 segundo
    
}

void exibir_temperatura(float temperatura) { //Função pra exibir temperatura no display
    char mensagem[30];
    snprintf(mensagem, sizeof(mensagem), "Temperatura %.0f", temperatura);
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0);
    ssd1306_draw_string(&ssd, mensagem, 2, 30);
    ssd1306_send_data(&ssd);
}

void verificar_temperatura(float temperatura) { //Função pra exibir temperatura e mandar comando adição gelo ou água
    char mensagem[30];

    if (temperatura >= 0 && temperatura <= 4) {
        snprintf(mensagem, sizeof(mensagem), "Temperatura %.0f", temperatura);
        ssd1306_fill(&ssd, false);
        ssd1306_draw_string(&ssd, mensagem, 2, 30);
        ssd1306_send_data(&ssd);

    } else if (temperatura > 4) {
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0);
        ssd1306_draw_string(&ssd, "Acima de 4", 10, 20);
        ssd1306_draw_string(&ssd, "Adicione gelo", 10, 30);
        ssd1306_send_data(&ssd);
        
        
    } else {
        ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0);
        ssd1306_draw_string(&ssd, "Abaixo de zero.", 10, 20);
        ssd1306_draw_string(&ssd, "Adicione agua!", 10, 40);
        ssd1306_send_data(&ssd);

    }
    
}

void gpio_irq_handler(uint gpio, uint32_t events) { //Função de interrupção com deboucing
    static uint32_t last_press_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (current_time - last_press_time < 300) return; // Ignora acionamentos rápidos
    last_press_time = current_time;

    if (gpio == PIN_BOTAO_A) {
        alarme_ativo = false;
        float temperatura;
        verificar_temperatura(temperatura);
    }

    if (gpio == PIN_BOTAO_B) {
        alarme_ativo = false;
        float temperatura;
        verificar_temperatura(temperatura);
        
    }
}
