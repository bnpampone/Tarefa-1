#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pio_matrix.h"
#include <stdbool.h>

#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13
#define BUTTON_A 5
#define BUTTON_B 6
#define DEBOUNCE_TIME 200  // Tempo para debounce

static int current_number = 0;
static const char numbers[10][25][6] = {
    { "000000", "222200", "222200", "222200", "000000", // Matriz 0
      "000000", "222200", "000000", "222200", "000000",
      "000000", "222200", "000000", "222200", "000000",
      "000000", "222200", "000000", "222200", "000000",
      "000000", "222200", "222200", "222200", "000000" },
    
    { "000000", "000000", "222200", "000000", "000000", // Matriz 1
      "000000", "222200", "222200", "000000", "000000",
      "000000", "000000", "222200", "000000", "000000",
      "000000", "000000", "222200", "000000", "000000",
      "000000", "000000", "222200", "000000", "000000" },

    { "000000", "222200", "222200", "222200", "000000", // Matriz 2
      "000000", "000000", "000000", "222200", "000000",
      "000000", "222200", "222200", "222200", "000000",
      "000000", "222200", "000000", "000000", "000000",
      "000000", "222200", "222200", "222200", "000000" },

    { "000000", "222200", "222200", "222200", "000000", // Matriz 3
      "000000", "000000", "000000", "222200", "000000",
      "000000", "222200", "222200", "222200", "000000",
      "000000", "000000", "000000", "222200", "000000",
      "000000", "222200", "222200", "222200", "000000" },

    { "000000", "222200", "000000", "222200", "000000", // Matriz 4
      "000000", "222200", "000000", "222200", "000000",
      "000000", "222200", "222200", "222200", "000000",
      "000000", "000000", "000000", "222200", "000000",
      "000000", "000000", "000000", "222200", "000000" },

    {   "000000", "222200", "222200", "222200", "000000", //Matriz 5
        "000000", "222200", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000",
        "000000", "000000", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000"},

    {   "000000", "222200", "222200", "222200", "000000", //Matriz 6
        "000000", "222200", "000000", "000000", "000000",
        "000000", "222200", "222200", "222200", "000000",
        "000000", "222200", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000"},

    {   "000000", "222200", "222200", "222200", "000000", //Matriz 7
        "000000", "222200", "000000", "222200", "000000",
        "000000", "222200", "000000", "222200", "000000",
        "000000", "000000", "000000", "222200", "000000",
        "000000", "000000", "000000", "222200", "000000"},

    {   "000000", "222200", "222200", "222200", "000000", //Matriz 8
        "000000", "222200", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000",
        "000000", "222200", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000"},

    {   "000000", "222200", "222200", "222200", "000000", //Matriz 9
        "000000", "222200", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000",
        "000000", "000000", "000000", "222200", "000000",
        "000000", "222200", "222200", "222200", "000000"}
};

// Controle de debounce e interrupção para botões
volatile bool button_a_pressed = false;
volatile bool button_b_pressed = false;

void button_a_irq(uint gpio, uint32_t events) {
    static uint32_t last_press_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    if (now - last_press_time > DEBOUNCE_TIME) {
        button_a_pressed = true;
        last_press_time = now;
    }
}

void button_b_irq(uint gpio, uint32_t events) {
    static uint32_t last_press_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    if (now - last_press_time > DEBOUNCE_TIME) {
        button_b_pressed = true;
        last_press_time = now;
    }
}

// LED vermelho piscando a 5Hz (sem bloquear o código)
void blink_red_led() {
    static uint32_t last_toggle_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (now - last_toggle_time >= 100) { // Alterna a cada 100ms (5Hz)
        gpio_put(LED_RED, !gpio_get(LED_RED));  
        last_toggle_time = now;
    }
}

int main() {
    stdio_init_all();
    
    // Inicializa os LEDs
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    
    // Inicializa os botões com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_edge_rise(BUTTON_A, true);
    gpio_set_irq_callback(&button_a_irq);
    irq_set_enabled(IO_IRQ_BANK0, true);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_edge_rise(BUTTON_B, true);
    gpio_set_irq_callback(&button_b_irq);
    irq_set_enabled(IO_IRQ_BANK0, true);

    // Inicializa a matriz de LED
    led_matrix_init();

    while (true) {
        blink_red_led();

        if (button_a_pressed) {
            if (current_number < 9) {
                current_number++;
            }
            button_a_pressed = false;
        }
        
        if (button_b_pressed) {
            if (current_number > 0) {
                current_number--;
            }
            button_b_pressed = false;
        }

        // Atualiza a matriz de LEDs com o número atual
        led_matrix_draw_animation(numbers[current_number]);
        sleep_ms(10);
    }
}
