#include "led-task/led-task.h"
#include <stdbool.h>
#include "systime/systime.h"
#include "platform/platform.h"

static led_state_t led_state;
static uint32_t led_period_ms;
static uint64_t led_last_toggle_us;
static bool led_level;

void led_task_init(void)
{
    led_period_ms = 1000;
    led_task_set_state(LED_STATE_BLINK);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;

    switch (state)
    {
        case LED_STATE_OFF:
            led_level = false;
            platform_led_set(false);
            break;

        case LED_STATE_ON:
            led_level = true;
            platform_led_set(true);
            break;

        case LED_STATE_BLINK:
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
                platform_led_set(led_level);
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
