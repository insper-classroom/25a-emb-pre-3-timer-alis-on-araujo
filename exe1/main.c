#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;  // Flag para detectar pressionamento do botão
struct repeating_timer timer;

bool timer_callback(struct repeating_timer *t) {
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));  // Alterna o estado do LED
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge (botão pressionado)
        flag_f_r = 1;
    }
}

int main() {
    stdio_init_all();

    // Inicializa o LED
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);  // Garante que o LED comece apagado

    // Inicializa o botão
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    // Configura a interrupção do botão
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    bool timer_running = false;  // Variável local para controlar o estado do timer

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0;  // Reseta a flag

            if (!timer_running) {
                // Inicia o timer repetitivo
                add_repeating_timer_ms(500, timer_callback, NULL, &timer);
                timer_running = true;
            } else {
                // Cancela o timer repetitivo
                cancel_repeating_timer(&timer);
                timer_running = false;
                gpio_put(LED_PIN_R, 0);  // Garante que o LED termine apagado
            }
        }
    }
}