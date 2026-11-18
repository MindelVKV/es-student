#include <stdio.h>
#include "pico/stdlib.h"

#include "stdio-text-protocol/stdio-text-protocol.h"
#include "api.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"
#include "profiling/profiling.h"
#include "logging/log.h"
#include "firmware.h"

// РџРµСЂРµС‡РёСЃР»СЏРµРјС‹Р№ С‚РёРї РЅРѕРјРµСЂРѕРІ СЃРµРєСѓРЅРґРѕРјРµСЂРѕРІ
typedef enum {
	STOPWATCH_LOOP,
	STOPWATCH_COMMAND,
	STOPWATCH_LED,
	STOPWATCH_BUTTON,
	STOPWATCH_PI,
	STOPWATCH_COUNT
} stopwatch_id_t;

int main(void)
{
	stdio_init_all();
	stdio_text_protocol_init();

	led_task_init();
	button_task_init();
	pi_task_init();

	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ РїСЂРѕС„РёР»РёСЂРѕРІС‰РёРєР°
	profiling_init();
	profiling_stopwatch_init(STOPWATCH_LOOP,    "loop");
	profiling_stopwatch_init(STOPWATCH_COMMAND, "command");
	profiling_stopwatch_init(STOPWATCH_LED,     "led");
	profiling_stopwatch_init(STOPWATCH_BUTTON,  "button");
	profiling_stopwatch_init(STOPWATCH_PI,      "pi");

	// Р›РѕРіРёСЂРѕРІР°РЅРёРµ РёРјРµРЅРё РїСЂРѕС€РёРІРєРё (РєР°Рє С‚СЂРµР±СѓРµС‚ Р·Р°РґР°РЅРёРµ)
	LOG_INF("%s %s\n", FIRMWARE_PROJECT, FIRMWARE_VERSION);

	while (true)
	{
		profiling_start(STOPWATCH_LOOP);

		profiling_start(STOPWATCH_COMMAND);
		const command_t *command = stdio_text_protocol_handle();
		if (command != NULL) {
			api_handle(command);
		}
		profiling_stop(STOPWATCH_COMMAND);

		profiling_start(STOPWATCH_LED);
		led_task_handle();
		profiling_stop(STOPWATCH_LED);

		profiling_start(STOPWATCH_BUTTON);
		button_task_handle();
		profiling_stop(STOPWATCH_BUTTON);

		profiling_start(STOPWATCH_PI);
		pi_task_handle();
		profiling_stop(STOPWATCH_PI);

		profiling_stop(STOPWATCH_LOOP);
	}
}

