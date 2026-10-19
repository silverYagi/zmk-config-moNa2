/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/settings/settings.h>
#include <zmk/behavior.h>

extern const struct test_settings_handler mona2_legacy_behavior;
static ssize_t read_name(void *arg, void *buffer, size_t size) {
    memcpy(buffer, arg, size);
    return size;
}
static int load(const char *key, const char *name) {
    return mona2_legacy_behavior.set(key, strlen(name), read_name, (void *)name);
}

int main(void) {
    assert(zmk_behavior_find_behavior_name_from_local_id(1) == NULL);
    assert(!strcmp(zmk_behavior_find_behavior_name_from_local_id(0x7101), "key_press"));
    /* 配列を先に読む場合も、全設定を読んだ後の再検索で復旧する。 */
    assert(load("local_id/1", "key_press") == 0);
    assert(load("local_id/2", "transparent") == 0);
    assert(!strcmp(zmk_behavior_find_behavior_name_from_local_id(1), "key_press"));
    assert(!strcmp(zmk_behavior_find_behavior_name_from_local_id(2), "transparent"));
    assert(zmk_behavior_find_behavior_name_from_local_id(999) == NULL);
    assert(load("local_id/3", "removed_behavior") == 0);
    assert(zmk_behavior_find_behavior_name_from_local_id(3) == NULL);
    /* 番号が衝突した場合も、現行CRC16の意味を変えない。 */
    assert(load("local_id/28929", "transparent") == 0);
    assert(!strcmp(zmk_behavior_find_behavior_name_from_local_id(0x7101), "key_press"));
    assert(load("local_id/0", "key_press") == -EINVAL);
    assert(load("local_id/65536", "key_press") == -EINVAL);
    assert(load("local_id/1/extra", "key_press") == -EINVAL);
    assert(load("local_id/no-number", "key_press") == -EINVAL);
    assert(load("other/1", "key_press") == 0);
    assert(load("local_id/1", "key_press") == 0);
    for (int i = 10; i < 15; i++) {
        char key[32];
        snprintf(key, sizeof(key), "local_id/%d", i);
        assert(load(key, "key_press") == 0);
    }
    assert(load("local_id/99", "key_press") == -ENOMEM);
    puts("旧テーブルID・現行CRC16・読み込み順・未知ID・境界値の照合成功");
    return 0;
}
