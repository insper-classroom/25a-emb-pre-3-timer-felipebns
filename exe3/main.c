#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_button = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R && events & GPIO_IRQ_EDGE_FALL) {
        flag_button = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); // Garante que começa apagado

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while (true) {
        if (flag_button) {
            uint32_t start_ms = to_ms_since_boot(get_absolute_time());
            while (!gpio_get(BTN_PIN_R)) {
                tight_loop_contents();
            }
            uint32_t delta = to_ms_since_boot(get_absolute_time()) - start_ms;
            if (delta > 500) {
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
            }
            flag_button = 0;
        }
    }
}