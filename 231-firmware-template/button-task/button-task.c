#include "button-task.h"
#include "led-task.h"
#include "pico/stdlib.h"`r`n#include "systime/systime.h"

#define BUTTON_PIN 15
#define DEBOUNCE_TIME_US 20000 // 20 РјСЃ

// РЎРѕСЃС‚РѕСЏРЅРёСЏ РјР°С€РёРЅС‹ РєРЅРѕРїРєРё
typedef enum {
    BUTTON_STATE_RELEASED,    // РѕС‚РїСѓС‰РµРЅР°
    BUTTON_STATE_PRESS_MAYBE, // РЅР°Р¶Р°С‚РёРµ?
    BUTTON_STATE_PRESSED,     // РЅР°Р¶Р°С‚Р°
    BUTTON_STATE_RELEASE_MAYBE // РѕС‚РїСѓСЃРєР°РЅРёРµ?
} button_state_t;

static button_state_t button_state;
static uint64_t button_timer_us;
static uint32_t press_count;

void button_task_init(void)
{
    // РќР°СЃС‚СЂРѕР№РєР° РІС‹РІРѕРґР° РЅР° РІС…РѕРґ СЃ РїРѕРґС‚СЏР¶РєРѕР№ Рє РїРёС‚Р°РЅРёСЋ
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    button_state = BUTTON_STATE_RELEASED;
    button_timer_us = 0;
    press_count = 0;
}

void button_task_handle(void)
{
    // РќР°Р¶Р°С‚Р°СЏ РєРЅРѕРїРєР° Р·Р°РјС‹РєР°РµС‚ РІС‹РІРѕРґ РЅР° Р·РµРјР»СЋ Рё РґР°РµС‚ 0 (false)
    bool is_level_pressed = !gpio_get(BUTTON_PIN);
    uint64_t now_us = systime_us();

    switch (button_state)
    {
        case BUTTON_STATE_RELEASED:
            if (is_level_pressed)
            {
                button_timer_us = now_us;
                button_state = BUTTON_STATE_PRESS_MAYBE;
            }
            break;

        case BUTTON_STATE_PRESS_MAYBE:
            if (!is_level_pressed)
            {
                // Р­С‚Рѕ Р±С‹Р» РґСЂРµР±РµР·Рі
                button_state = BUTTON_STATE_RELEASED;
            }
            else if (now_us - button_timer_us >= DEBOUNCE_TIME_US)
            {
                // РќР°Р¶Р°С‚РёРµ РїРѕРґС‚РІРµСЂР¶РґРµРЅРѕ
                button_state = BUTTON_STATE_PRESSED;
                press_count++;

                // РџРµСЂРµРєР»СЋС‡Р°РµРј СЃРІРµС‚РѕРґРёРѕРґ
                led_task_next_state();
            }
            break;

        case BUTTON_STATE_PRESSED:
            if (!is_level_pressed)
            {
                button_timer_us = now_us;
                button_state = BUTTON_STATE_RELEASE_MAYBE;
            }
            break;

        case BUTTON_STATE_RELEASE_MAYBE:
            if (is_level_pressed)
            {
                // Р­С‚Рѕ Р±С‹Р» РґСЂРµР±РµР·Рі РїСЂРё РѕС‚РїСѓСЃРєР°РЅРёРё
                button_state = BUTTON_STATE_PRESSED;
            }
            else if (now_us - button_timer_us >= DEBOUNCE_TIME_US)
            {
                // РћС‚РїСѓСЃРєР°РЅРёРµ РїРѕРґС‚РІРµСЂР¶РґРµРЅРѕ
                button_state = BUTTON_STATE_RELEASED;
            }
            break;
    }
}

bool button_task_is_pressed(void)
{
    return (button_state == BUTTON_STATE_PRESSED || button_state == BUTTON_STATE_RELEASE_MAYBE);
}

uint32_t button_task_get_press_count(void)
{
    return press_count;
}

