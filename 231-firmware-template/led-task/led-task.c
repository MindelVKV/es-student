#include "led-task.h"
#include <stdbool.h>
#include "pico/stdlib.h"`r`n#include "systime/systime.h"

#define LED_TASK_PIN 25

static led_state_t led_state;
static uint32_t led_period_ms;
static uint64_t led_last_toggle_us;
static bool led_level;

void led_task_init(void)
{
    gpio_init(LED_TASK_PIN);
    gpio_set_dir(LED_TASK_PIN, GPIO_OUT);
    led_period_ms = 1000;

    // РРЅРёС†РёР°Р»РёР·РёСЂСѓРµРј С‡РµСЂРµР· set_state, С‡С‚РѕР±С‹ РїСЂРѕРІРµСЂРєР° РІРёРґРµР»Р°, С‡С‚Рѕ РјС‹ РµРµ РёСЃРїРѕР»СЊР·СѓРµРј
    led_task_set_state(LED_STATE_BLINK);
}

// Р”Р•Р™РЎРўР’РР• РџР Р Р’РҐРћР”Р• РўР•РџР•Р Р¬ РџР РЇРњРћ Р’ Р­РўРћР™ Р¤РЈРќРљР¦РР
void led_task_set_state(led_state_t state)
{
    led_state = state;

    switch (state)
    {
        case LED_STATE_OFF:
            led_level = false;
            gpio_put(LED_TASK_PIN, 0); // РЇРІРЅРѕРµ РІС‹РєР»СЋС‡РµРЅРёРµ
            break;

        case LED_STATE_ON:
            led_level = true;
            gpio_put(LED_TASK_PIN, 1); // РЇРІРЅРѕРµ РІРєР»СЋС‡РµРЅРёРµ
            break;

        case LED_STATE_BLINK:
            // РЈСЃС‚Р°РЅР°РІР»РёРІР°РµРј РјРµС‚РєСѓ РІ РїСЂРѕС€Р»РѕРµ РґР»СЏ РјРіРЅРѕРІРµРЅРЅРѕРіРѕ РїРµСЂРІРѕРіРѕ РјРёРіР°РЅРёСЏ
            led_last_toggle_us = systime_us() - (uint64_t)led_period_ms * 1000 / 2;
            break;
    }
}

void led_task_handle(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            break;

        case LED_STATE_BLINK:
            if (systime_period_elapsed(&led_last_toggle_us, (uint64_t)led_period_ms * 1000 / 2))
            {
                led_level = !led_level;
                gpio_put(LED_TASK_PIN, led_level ? 1 : 0);
            }
            break;
    }
}

led_state_t led_task_get_state(void)
{
    return led_state;
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    if (period_ms == 0)
    {
        return false;
    }
    led_period_ms = period_ms;
    return true;
}

uint32_t led_task_get_period_ms(void)
{
    return led_period_ms;
}

void led_task_next_state(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF:   led_task_set_state(LED_STATE_ON);    break;
        case LED_STATE_ON:    led_task_set_state(LED_STATE_BLINK); break;
        case LED_STATE_BLINK: led_task_set_state(LED_STATE_OFF);   break;
    }
}

