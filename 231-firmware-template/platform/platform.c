#include "platform/platform.h"
#include "pico/stdlib.h"

#define PLATFORM_LED_PIN    25
#define PLATFORM_BUTTON_PIN 15

void platform_init(void)
{
    stdio_init_all();

    gpio_init(PLATFORM_LED_PIN);
    gpio_set_dir(PLATFORM_LED_PIN, GPIO_OUT);

    gpio_init(PLATFORM_BUTTON_PIN);
    gpio_set_dir(PLATFORM_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(PLATFORM_BUTTON_PIN);
}

void platform_led_set(bool on)
{
    gpio_put(PLATFORM_LED_PIN, on ? 1 : 0);
}

bool platform_button_read(void)
{
    // нажатая кнопка замыкает вывод на землю
    return !gpio_get(PLATFORM_BUTTON_PIN);
}
