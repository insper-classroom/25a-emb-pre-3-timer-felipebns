#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

const int BTN_PIN_G = 26;
const int LED_PIN_G = 6;

volatile int g_timer_0 = 0;
volatile int g_timer_1 = 0;

volatile int flag_f_r = 0;
volatile int flag_f_g = 0;

bool timer_0_callback(repeating_timer_t *rt) {
    g_timer_0 = 1;
    return true; // keep repeating
}

bool timer_1_callback(repeating_timer_t *rt) {
    g_timer_1 = 1;
    return true; // keep repeating
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4 && gpio == BTN_PIN_R) { // fall edge
        flag_f_r = !flag_f_r;
    } else if (events == 0x4 && gpio == BTN_PIN_G) { // fall edge
        flag_f_g = !flag_f_g;
    } 
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    repeating_timer_t timer_0;
    repeating_timer_t timer_1;

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int ja_ligou = 0;
    int ja_ligou2 = 0;

    while (true) {

        if (flag_f_r && !ja_ligou) {
            add_repeating_timer_ms(500, timer_0_callback, NULL, &timer_0);
            ja_ligou = 1;
        } else if (!flag_f_r && ja_ligou) {
            cancel_repeating_timer(&timer_0);
            ja_ligou = 0;
            gpio_put(LED_PIN_R, 0);
        }

        if (flag_f_g && !ja_ligou2) {
            add_repeating_timer_ms(250, timer_1_callback, NULL, &timer_1);
            ja_ligou2 = 1;
        } else if (!flag_f_g && ja_ligou2) {
            cancel_repeating_timer(&timer_1);
            ja_ligou2 = 0;
            gpio_put(LED_PIN_G, 0);
        }

        if (g_timer_0) {
            printf("Hello timer 0 \n");
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
            g_timer_0 = 0;
        } 


        if (g_timer_1) {
            printf("Hello timer 1 \n");
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
            g_timer_1 = 0;
        } 
        
    }
}
