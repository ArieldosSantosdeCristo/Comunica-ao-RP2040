#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h"  // Biblioteca para controle dos LEDs WS2812

#include "inc/ssd1306.h"      // Biblioteca para controle do display OLED


#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define LED_RED 11
#define LED_BLUE 12
#define LED_GREEN 13
#define BUTTON_A 5
#define BUTTON_B 6
#define MATRIX_WS2812 7
#define OLED_SDA 14
#define OLED_SCL 15
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Variáveis globais para controle de LEDs
volatile bool led_green_state = false;
volatile bool led_blue_state = false;

// Função de debounce
bool debounce(uint gpio) {
    sleep_ms(50);
    return gpio_get(gpio) == 0;
}

// Interrupções dos botões
void button_a_pressed(uint gpio, uint32_t events) {
    if (debounce(BUTTON_A)) {
        led_green_state = !led_green_state;
        gpio_put(LED_GREEN, led_green_state);
        printf("LED Verde: %s\n", led_green_state ? "ON" : "OFF");
    }
}

void button_b_pressed(uint gpio, uint32_t events) {
    if (debounce(BUTTON_B)) {
        led_blue_state = !led_blue_state;
        gpio_put(LED_BLUE, led_blue_state);
        printf("LED Azul: %s\n", led_blue_state ? "ON" : "OFF");
    }
}

// Função para exibir um número na matriz de LEDs
void display_number_on_matrix(uint8_t number) {
    if (number >= 0 && number <= 9) {
        uint32_t color = 0x00FF00;  // Verde para números
        ws2812_set_pixel(number, color);
    }
}

int main() {
    stdio_init_all();
    
    // Inicializa GPIOs
    gpio_init(LED_RED);
    gpio_init(LED_BLUE);
    gpio_init(LED_GREEN);
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);

    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);

    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);
    
    // Configura interrupções
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_a_pressed);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_b_pressed);

    // Inicializa display OLED
    
    // I2C Initialisation. Using it at 400Khz.
        
    i2c_init(I2C_PORT, 400 * 1000);
      
        gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
        gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
        gpio_pull_up(I2C_SDA); // Pull up the data line
        gpio_pull_up(I2C_SCL); // Pull up the clock line
        ssd1306_t ssd; // Inicializa a estrutura do display
        ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
        ssd1306_config(&ssd); // Configura o display
        ssd1306_send_data(&ssd); // Envia os dados para o display
      
        // Limpa o display. O display inicia com todos os pixels apagados.
        ssd1306_fill(&ssd, false);
        ssd1306_send_data(&ssd);
   
   
        // Inicializa matriz de LEDs WS2812
    
        
        bool cor = true;
        while (true)
        {
          cor = !cor;
          // Atualiza o conteúdo do display com animações
          ssd1306_fill(&ssd, !cor); // Limpa o display
          ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
          ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 10); // Desenha uma string
          ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30); // Desenha uma string
          ssd1306_draw_string(&ssd, "PROF WILTON", 15, 48); // Desenha uma string      
          ssd1306_send_data(&ssd); // Atualiza o display
      
          sleep_ms(1000);
        }
      }