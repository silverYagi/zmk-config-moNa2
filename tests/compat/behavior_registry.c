/* SPDX-License-Identifier: MIT */
#include <stddef.h>
#include <string.h>
#include <zmk/behavior.h>

/* 旧版の連番とは異なる、新版のCRC16を想定したレジストリー。 */
const char *zmk_behavior_find_behavior_name_from_local_id(zmk_behavior_local_id_t id) {
    if (id == 0x7101) { return "key_press"; }
    if (id == 0x7102) { return "transparent"; }
    return NULL;
}
zmk_behavior_local_id_t zmk_behavior_get_local_id(const char *name) {
    if (!strcmp(name, "key_press")) { return 0x7101; }
    if (!strcmp(name, "transparent")) { return 0x7102; }
    return UINT16_MAX;
}
