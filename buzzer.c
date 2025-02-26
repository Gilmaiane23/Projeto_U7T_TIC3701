//Biblioteca da atividade antividade 1, disponievel em: https://github.com/Gilmaiane23/atividade1/blob/main/buzzer.h

#include <pico/stdlib.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "buzzer.h"

//Definições buzzer
#define BUZZER_PIN 10


// Função PWM para buzzer
void setup_buzzer(uint buzzer_pin, uint frequency) {
    // Configura o pino do buzzer para PWM
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    
    // Obtém o slice do pino
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);

    // Calcula o divisor de clock
    uint clock = 125000000;  // Clock padrão do Raspberry Pi Pico em Hz
    uint divider = clock / (frequency * 4096);
    if (divider < 1) divider = 1;  // Garante que o divisor seja válido
    pwm_set_clkdiv(slice_num, divider);

    // Configura o período do PWM
    pwm_set_wrap(slice_num, 4095);

    // Define o duty cycle como 0% (desligado)
    pwm_set_gpio_level(buzzer_pin, 0);

    // Não ativa o PWM ainda, apenas configura o slice
    pwm_set_enabled(slice_num, false);
}

//Função para ligar buzzer
void start_buzzer(uint buzzer_pin, uint frequency) {
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM); // Configura o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);

    // Configura a frequência do PWM
    uint clock = 125000000; // Clock base do sistema
    uint divider = clock / (frequency * 4096);
    if (divider < 1) divider = 1;
    pwm_set_clkdiv(slice_num, divider);

    pwm_set_wrap(slice_num, 4095);                // Resolução do PWM (12 bits)
    pwm_set_gpio_level(buzzer_pin, 2048);         // Define o duty cycle (50%)
    pwm_set_enabled(slice_num, true);            // Habilita o PWM
}


//Função para desligar buzzer
void stop_buzzer(uint buzzer_pin) {
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    pwm_set_gpio_level(buzzer_pin, 0);           // Define o duty cycle como 0 (sem som)
    pwm_set_enabled(slice_num, false);   
    sleep_ms(20); 

}