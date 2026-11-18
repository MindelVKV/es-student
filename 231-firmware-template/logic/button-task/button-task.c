#include "button-task/button-task.h"
#include "systime/systime.h"
#include "platform/platform.h"

#define DEBOUNCE_TIME_US 20000

typedef enum {
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESS_MAYBE,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_RELEASE_MAYBE
} button_state_t;

static button_state_t button_state;
static uint64_t button_timer_us;
static uint32_t press_count;
static button_task_callback_t on_press_callback;

void button_task_init(button_task_callback_t on_press)
{
    button_state = BUTTON_STATE_RELEASED;
    button_timer_us = 0;
    press_count = 0;
    on_press_callback = on_press;
}

void button_task_handle(void)
{
    bool is_level_pressed = platform_button_read();
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
                button_state = BUTTON_STATE_RELEASED;
            }
            else if (now_us - button_timer_us >= DEBOUNCE_TIME_US)
            {
                button_state = BUTTON_STATE_PRESSED;
                press_count++;

                if (on_press_callback != NULL)
                {
                    on_press_callback();
                }
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
                button_state = BUTTON_STATE_PRESSED;
            }
            else if (now_us - button_timer_us >= DEBOUNCE_TIME_US)
            {
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
