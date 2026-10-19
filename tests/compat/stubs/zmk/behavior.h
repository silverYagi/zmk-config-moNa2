/* SPDX-License-Identifier: MIT */
#pragma once
#include <stdint.h>
typedef uint16_t zmk_behavior_local_id_t;
const char *zmk_behavior_find_behavior_name_from_local_id(zmk_behavior_local_id_t id);
zmk_behavior_local_id_t zmk_behavior_get_local_id(const char *name);
