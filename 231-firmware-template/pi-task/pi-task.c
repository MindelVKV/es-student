#include "pi-task.h"
#include "pico/stdlib.h"`r`n#include "systime/systime.h"

// РћРєРѕР»Рѕ 100 С‡Р»РµРЅРѕРІ СЂСЏРґР°: 100 * 4.7 РјРєСЃ в‰€ 0.47 РјСЃ, СѓРєР»Р°РґС‹РІР°РµРјСЃСЏ РІ 0.5 РјСЃ
#define PI_TERMS_PER_CALL 100

static pi_state_t pi_state;

static uint32_t pi_k;      // С‚РµРєСѓС‰РёР№ С‡Р»РµРЅ
static uint32_t pi_n;      // РІСЃРµРіРѕ С‡Р»РµРЅРѕРІ
static double pi_sum;
static double pi_sign;

static double pi_result;
static uint32_t pi_elapsed_ms;

static uint32_t pi_start_time_us;

void pi_task_init(void)
{
    pi_state = PI_STATE_NOT_STARTED;
    pi_k = 0;
    pi_n = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_result = 0.0;
    pi_elapsed_ms = 0;
}

bool pi_task_start(uint32_t terms)
{
    if (pi_state == PI_STATE_RUNNING)
    {
        return false; // СѓР¶Рµ СЃС‡РёС‚Р°РµС‚
    }

    if (terms == 0)
    {
        return false; // РЅРµС‡РµРіРѕ СЃС‡РёС‚Р°С‚СЊ
    }

    pi_n = terms;
    pi_k = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    pi_state = PI_STATE_RUNNING;
    pi_start_time_us = (uint32_t)systime_us();

    return true;
}

void pi_task_handle(void)
{
    if (pi_state != PI_STATE_RUNNING)
    {
        return;
    }

    // РћРїСЂРµРґРµР»СЏРµРј РєРѕРЅРµС† РїРѕСЂС†РёРё
    uint32_t end;
    // Р—Р°С‰РёС‚Р° РѕС‚ РїРµСЂРµРїРѕР»РЅРµРЅРёСЏ 32-Р±РёС‚РЅРѕРіРѕ СЃР»РѕР¶РµРЅРёСЏ (Р¤5)
    if (pi_n - pi_k < PI_TERMS_PER_CALL)
    {
        end = pi_n;
    }
    else
    {
        end = pi_k + PI_TERMS_PER_CALL;
    }

    // РЎС‡РёС‚Р°РµРј СЂСЏРґ Р›РµР№Р±РЅРёС†Р°
    for (; pi_k < end; pi_k++)
    {
        pi_sum += pi_sign / (2.0 * (double)pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    // РџСЂРѕРІРµСЂСЏРµРј, Р·Р°РєРѕРЅС‡РёР»Рё Р»Рё
    if (pi_k == pi_n)
    {
        pi_result = 4.0 * pi_sum;
        pi_elapsed_ms = ((uint32_t)systime_us() - pi_start_time_us) / 1000;
        pi_state = PI_STATE_READY;
    }
}

pi_state_t pi_task_get_state(void)
{
    return pi_state;
}

bool pi_task_get_result(double *pi_value, uint32_t *elapsed_ms)
{
    if (pi_state != PI_STATE_READY)
    {
        return false;
    }

    if (pi_value != NULL)
    {
        *pi_value = pi_result;
    }
    if (elapsed_ms != NULL)
    {
        *elapsed_ms = pi_elapsed_ms;
    }

    return true;
}

