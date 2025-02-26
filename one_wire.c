/*#include "pico-onewire/onewire/main.h"


// Inicializa o barramento 1-Wire
    OneWireBus *owb = owb_rmt_initialize(ONE_WIRE_GPIO);
    owb_use_crc(owb, true); // Ativa verificação de CRC

    // Busca dispositivos 1-Wire conectados
    OneWireBus_ROMCode rom_code;
    if (owb_read_rom(owb, &rom_code) == OWB_STATUS_OK) {
        printf("Sensor encontrado: %016llX\n", rom_code);
    } else {
        printf("Nenhum sensor detectado!\n");
        return 1;
    }

    // Inicializa o sensor DS18B20
    DS18B20_Info *ds18b20 = ds18b20_malloc(); // Aloca espaço para o sensor
    ds18b20_init(ds18b20, owb, rom_code);
    ds18b20_use_crc(ds18b20, true); // Usa CRC para segurança na leitura
    ds18b20_set_resolution(ds18b20, DS18B20_RESOLUTION_12_BIT); // Define a resolução

    ds18b20_convert_all(owb); // Inicia conversão de temperatura
    sleep_ms(750); // Aguarda a conversão (750ms para 12-bit)

    if (ds18b20_read_temp(ds18b20, &temp_c) == DS18B20_OK) {
            printf("Temperatura: %.2f°C\n", temp_c);
        } else {
            printf("Erro na leitura do sensor\n");
        }

        ds18b20_free(ds18b20); // Libera a memória do sensor*/
