#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile uint32_t press_start_time = 0;
volatile bool led_state = false;
volatile bool button_held = false;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R) {
        if (events & GPIO_IRQ_EDGE_FALL) { // Botão pressionado
            press_start_time = to_ms_since_boot(get_absolute_time());
            button_held = true;
        } else if (events & GPIO_IRQ_EDGE_RISE) { // Botão solto
            if (button_held) {
                uint32_t press_duration = to_ms_since_boot(get_absolute_time()) - press_start_time;
                if (press_duration > 500) {
                    led_state = !led_state;
                    gpio_put(LED_PIN_R, led_state);
                }
                button_held = false;
            }
        }
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
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        tight_loop_contents(); // Mantém o loop ativo
    }
}