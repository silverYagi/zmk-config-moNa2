/* SPDX-License-Identifier: MIT */
#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
typedef ssize_t (*settings_read_cb)(void *, void *, size_t);
struct test_settings_handler {
    int (*set)(const char *, size_t, settings_read_cb, void *);
};
#define SETTINGS_STATIC_HANDLER_DEFINE(id, name, get, set_fn, commit, export) \
    const struct test_settings_handler id = {.set = set_fn}
static inline int settings_name_steq(const char *name, const char *key, const char **next) {
    size_t size = strlen(key);
    if (strncmp(name, key, size) || (name[size] != '\0' && name[size] != '/')) {
        return 0;
    }
    *next = name[size] == '/' ? name + size + 1 : NULL;
    return 1;
}
