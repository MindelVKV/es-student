#include "device/device.h"
#include <string.h>
#include "pico/stdlib.h"
#include "pico/unique_id.h"

void device_get_info(device_info_t *info)
{
    if (info == NULL) return;

    strcpy(info->board, "pico");
    pico_get_unique_board_id_string(info->serial, sizeof(info->serial));
    strcpy(info->manufacturer, "Raspberry Pi");
    strcpy(info->product, "RP2040");
    strcpy(info->chip_revision, "B2");
    strcpy(info->sdk_version, "2.1.0");
}
