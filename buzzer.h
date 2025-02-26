#ifndef BUZZER_H
#define BUZZER_H

void setup_buzzer(uint buzzer_pin, uint frequency);
void start_buzzer(uint buzzer_pin, uint frequency);
void stop_buzzer(uint buzzer_pin);

#endif