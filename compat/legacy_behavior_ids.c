/* SPDX-License-Identifier: MIT */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/settings/settings.h>
#include <zmk/behavior.h>

struct legacy_behavior_alias {
    zmk_behavior_local_id_t legacy;
    zmk_behavior_local_id_t current;
};

static struct legacy_behavior_alias aliases[CONFIG_MONA2_LEGACY_BEHAVIOR_ID_LIMIT];
static size_t alias_count;

const char *__real_zmk_behavior_find_behavior_name_from_local_id(zmk_behavior_local_id_t id);

/* 現在のCRC16を優先し、未解決の場合だけ旧テーブルの番号で補う。 */
const char *__wrap_zmk_behavior_find_behavior_name_from_local_id(zmk_behavior_local_id_t id) {
    const char *name = __real_zmk_behavior_find_behavior_name_from_local_id(id);
    if (name) {
        return name;
    }
    for (size_t i = 0; i < alias_count; i++) {
        if (aliases[i].legacy == id) {
            return __real_zmk_behavior_find_behavior_name_from_local_id(aliases[i].current);
        }
    }
    return NULL;
}

static int legacy_behavior_handle_set(const char *key, size_t len, settings_read_cb read_cb,
                                      void *cb_arg) {
    const char *next;
    if (!settings_name_steq(key, "local_id", &next) || !next) {
        return 0;
    }

    char *end;
    unsigned long legacy = strtoul(next, &end, 10);
    if (end == next || *end != '\0' || legacy == 0 || legacy > UINT16_MAX || len == 0 ||
        len >= 64) {
        return -EINVAL;
    }

    char name[64];
    int ret = read_cb(cb_arg, name, len);
    if (ret < 0) {
        return ret;
    }
    if ((size_t)ret != len || memchr(name, '\0', len)) {
        return -EINVAL;
    }
    name[len] = '\0';

    zmk_behavior_local_id_t current = zmk_behavior_get_local_id(name);
    const char *resolved = __real_zmk_behavior_find_behavior_name_from_local_id(current);
    if (!resolved || strcmp(resolved, name) != 0) {
        /* 既に存在しない動作は別の動作へ置き換えない。 */
        return 0;
    }

    for (size_t i = 0; i < alias_count; i++) {
        if (aliases[i].legacy == legacy) {
            aliases[i].current = current;
            return 0;
        }
    }
    if (alias_count >= CONFIG_MONA2_LEGACY_BEHAVIOR_ID_LIMIT) {
        return -ENOMEM;
    }
    aliases[alias_count++] = (struct legacy_behavior_alias){.legacy = legacy, .current = current};
    return 0;
}

/* CRC16方式では本体のbehavior設定ハンドラーは登録されない。読み取りだけを追加する。
 * keymapのcommitで未解決IDを再評価するため、設定の読み込み順にも依存しない。 */
SETTINGS_STATIC_HANDLER_DEFINE(mona2_legacy_behavior, "behavior", NULL,
                               legacy_behavior_handle_set, NULL, NULL);
