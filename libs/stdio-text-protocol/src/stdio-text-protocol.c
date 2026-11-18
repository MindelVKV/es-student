#include "stdio-text-protocol/stdio-text-protocol.h"

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

// Р’СЃС‘ СЃРѕСЃС‚РѕСЏРЅРёРµ РїСЂРѕС‚РѕРєРѕР»Р° Р·Р°РєСЂС‹С‚Рѕ РІ СЌС‚РѕРј С„Р°Р№Р»Рµ (static).
static char line[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint32_t line_len;
static bool line_overflow;

static command_t command;

void stdio_text_protocol_init(void)
{
    line_len = 0;
    line_overflow = false;
    line[0] = '\0';
}

// Р”РµР»РёС‚ СЃС‚СЂРѕРєСѓ РЅР° СЃР»РѕРІР° РЅР° РјРµСЃС‚Рµ: РїСЂРѕР±РµР»С‹ Р·Р°РјРµРЅСЏСЋС‚СЃСЏ '\0',
// РІ name Рё argv РїРёС€СѓС‚СЃСЏ СѓРєР°Р·Р°С‚РµР»Рё РЅР° РЅР°С‡Р°Р»Р° СЃР»РѕРІ.
static void split_line(void)
{
    command.name = NULL;
    command.argc = 0;
    command.truncated = line_overflow;

    uint32_t i = 0;

    // РїСЂРѕРїСѓСЃРєР°РµРј РІРµРґСѓС‰РёРµ РїСЂРѕР±РµР»С‹
    while (i < line_len && line[i] == ' ')
    {
        i++;
    }

    if (i >= line_len)
    {
        // РїСѓСЃС‚Р°СЏ СЃС‚СЂРѕРєР°
        command.name = NULL;
        return;
    }

    command.name = &line[i];

    while (i < line_len)
    {
        // СЃР»РѕРІРѕ
        while (i < line_len && line[i] != ' ')
        {
            i++;
        }

        // РєРѕРЅРµС† СЃР»РѕРІР°: РµСЃР»Рё СЌС‚Рѕ РЅРµ РєРѕРЅРµС† СЃС‚СЂРѕРєРё вЂ” СЃС‚Р°РІРёРј '\0'
        if (i < line_len)
        {
            line[i] = '\0';
            i++;
        }

        // РїСЂРѕРїСѓСЃРєР°РµРј РїСЂРѕР±РµР»С‹ РјРµР¶РґСѓ СЃР»РѕРІР°РјРё
        while (i < line_len && line[i] == ' ')
        {
            i++;
        }

        if (i >= line_len)
        {
            break;
        }

        // СЃР»РµРґСѓСЋС‰РµРµ СЃР»РѕРІРѕ вЂ” Р°СЂРіСѓРјРµРЅС‚
        if (command.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS)
        {
            command.argv[command.argc] = &line[i];
            command.argc++;
        }
        else
        {
            // Р°СЂРіСѓРјРµРЅС‚РѕРІ Р±РѕР»СЊС€Рµ, С‡РµРј РїРѕРјРµС‰Р°РµС‚СЃСЏ
            command.truncated = true;
            break;
        }
    }
}

const command_t *stdio_text_protocol_handle(void)
{
    // С‡РёС‚Р°РµРј СЃРёРјРІРѕР» Р±РµР· РѕР¶РёРґР°РЅРёСЏ; РЅРµС‚ СЃРёРјРІРѕР»Р° вЂ” РІРѕР·РІСЂР°С‰Р°РµРј NULL
    int c = getchar_timeout_us(0);
    if (c == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    // РєРѕРЅРµС† СЃС‚СЂРѕРєРё
    if (c == '\r' || c == '\n')
    {
        // РїСѓСЃС‚СѓСЋ СЃС‚СЂРѕРєСѓ РїСЂРѕРїСѓСЃРєР°РµРј
        if (line_len == 0 && !line_overflow)
        {
            return NULL;
        }

        // РїРµСЂРµРІРѕРґРёРј СЃС‚СЂРѕРєСѓ РІ С‚РµСЂРјРёРЅР°Р»Рµ
        putchar('\n');

        // Р·Р°РІРµСЂС€Р°РµРј Р±СѓС„РµСЂ
        line[line_len < STDIO_TEXT_PROTOCOL_LINE_SIZE ? line_len : STDIO_TEXT_PROTOCOL_LINE_SIZE - 1] = '\0';

        // РґРµР»РёРј СЃС‚СЂРѕРєСѓ РЅР° СЃР»РѕРІР°
        split_line();

        // РіРѕС‚РѕРІРёРј Р±СѓС„РµСЂ Рє СЃР»РµРґСѓСЋС‰РµР№ СЃС‚СЂРѕРєРµ
        line_len = 0;
        line_overflow = false;

        return command.name != NULL ? &command : NULL;
    }

    // Backspace: 0x08 (Ctrl+H) РёР»Рё 0x7f (DEL)
    if (c == 0x08 || c == 0x7f)
    {
        if (line_len > 0)
        {
            line_len--;
            // СЃС‚РёСЂР°РµРј СЃРёРјРІРѕР» РЅР° СЌРєСЂР°РЅРµ: С€Р°Рі РЅР°Р·Р°Рґ, РїСЂРѕР±РµР», С€Р°Рі РЅР°Р·Р°Рґ
            printf("\b \b");
        }
        return NULL;
    }

    // РїРµС‡Р°С‚РЅС‹Р№ СЃРёРјРІРѕР»: РєР»Р°РґС‘Рј РІ Р±СѓС„РµСЂ Рё РїРѕРІС‚РѕСЂСЏРµРј РІ С‚РµСЂРјРёРЅР°Р»
    if (c >= 0x20 && c < 0x7f)
    {
        if (line_len < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1)
        {
            line[line_len] = (char)c;
            line_len++;
            putchar(c);
        }
        else
        {
            // Р±СѓС„РµСЂ РїРѕР»РѕРЅ вЂ” РїРѕРјРµС‡Р°РµРј СЃС‚СЂРѕРєСѓ РєР°Рє РїРµСЂРµРїРѕР»РЅРµРЅРЅСѓСЋ
            line_overflow = true;
            putchar(c);
        }
    }

    return NULL;
}

