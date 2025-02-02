#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ws2818b.pio.h"

#define LED_COUNT 25
#define LED_PIN 7
#define RGB_PIN 13

int led_value = 0;

// Definição da estrutura do pixel
struct pixel_t {
    uint8_t G, R, B;
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t;

npLED_t leds[LED_COUNT];
PIO np_pio;
uint sm;

volatile int current_digit = 0;
absolute_time_t last_interrupt_time = 0;

// Matrizes para cada dígito 
const uint8_t digits[10][5][5][3] = {
    // Número 0
    {
        {{0, 0, 0}, {0,100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}  
    },
    // Número 1
    {
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}} 
    },
    // Número 2
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}  
    },
    // Número 3
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}} 
    },
    // Número 4
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}} 
    },
    // Número 5
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}} 
    },
    // Número 6
    {
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}} 
    },
    // Número 7
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}} 
    },
    // Número 8
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}} 
    },
    // Número 9
    {
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}}, 
        {{0, 0, 0}, {0, 100, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 100, 0}, {0, 0, 0}},    
        {{0, 0, 0}, {0, 100, 0}, {0, 100, 0}, {0, 0, 0}, {0, 0, 0}} 
    },
   
};

// Função de callback para interrupções
void gpio_callback(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();
    int64_t diff = absolute_time_diff_us(last_interrupt_time, now);

    if (diff < 250000) return; // Debounce de 150ms
    last_interrupt_time = now;

    if (gpio == 6) {
        current_digit = (current_digit + 1) % 10;
    } else if (gpio == 5) {
        current_digit = (current_digit - 1 + 10) % 10;
    }
}

void npSetLED(uint index, uint8_t r, uint8_t g, uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}

void npClear() {
    for (uint i = 0; i < LED_COUNT; i++) {
        npSetLED(i, 0, 0, 0);
    }
}


// Inicialização da matriz de LEDs
void npInit(uint pin) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
    npClear();
}


void npWrite() {
    for (uint i = 0; i < LED_COUNT; i++) {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100);
}

int getIndex(int x, int y) {
    if (y % 2 == 0) {
        return 24 - (y * 5 + x);
    } else {
        return 24 - (y * 5 + (4 - x));
    }
}

bool repeating_timer_callback(struct repeating_timer *t)
{
  led_value = !led_value;
  gpio_put(RGB_PIN, led_value);

  return true;
}

int main() {
    stdio_init_all();
    npInit(LED_PIN);

    // Configuração dos botões
    gpio_init(5);
    gpio_set_dir(5, GPIO_IN);
    gpio_pull_up(5);

    gpio_init(6);
    gpio_set_dir(6, GPIO_IN);
    gpio_pull_up(6);

    //Inicialização do RGB
    gpio_init(RGB_PIN);
    gpio_set_dir(RGB_PIN, GPIO_OUT);

    // Configuração das interrupções
    gpio_set_irq_enabled(5, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(6, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);

    // Configuração do timer
    struct repeating_timer timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);

    while (true) {
        for (int coluna = 0; coluna < 5; coluna++) {
            for (int linha = 0; linha < 5; linha++) {
                int posicao = getIndex(linha, coluna);
                npSetLED(posicao,
                    digits[current_digit][coluna][linha][0],
                    digits[current_digit][coluna][linha][1],
                    digits[current_digit][coluna][linha][2]);
            }
        }
        npWrite();
        sleep_ms(50);
    }
}