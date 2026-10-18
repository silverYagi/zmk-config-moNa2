/* SPDX-License-Identifier: MIT */

#include <zephyr/bluetooth/conn.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(mona2_ble_diagnostics, LOG_LEVEL_INF);

/* 接続先のアドレスや認証値を記録せず、中央・周辺の役割だけで識別する。 */
static int connection_role(struct bt_conn *conn) {
    struct bt_conn_info info;
    return bt_conn_get_info(conn, &info) == 0 ? info.role : -1;
}

static void diagnostics_connected(struct bt_conn *conn, uint8_t err) {
    LOG_INF("connected role=%d status=0x%02x", connection_role(conn), err);
}

static void diagnostics_disconnected(struct bt_conn *conn, uint8_t reason) {
    LOG_INF("disconnected role=%d reason=0x%02x", connection_role(conn), reason);
}

static void diagnostics_security_changed(struct bt_conn *conn, bt_security_t level,
                                         enum bt_security_err err) {
    LOG_INF("security role=%d level=%u err=%d", connection_role(conn), level, err);
}

BT_CONN_CB_DEFINE(mona2_diagnostics_callbacks) = {
    .connected = diagnostics_connected,
    .disconnected = diagnostics_disconnected,
    .security_changed = diagnostics_security_changed,
};

static void diagnostics_pairing_complete(struct bt_conn *conn, bool bonded) {
    LOG_INF("pairing_complete role=%d bonded=%d", connection_role(conn), bonded);
}

static void diagnostics_pairing_failed(struct bt_conn *conn, enum bt_security_err reason) {
    LOG_INF("pairing_failed role=%d reason=%d", connection_role(conn), reason);
}

/* 認証方式を変更しない通知専用コールバックを追加する。 */
static struct bt_conn_auth_info_cb diagnostics_auth_callbacks = {
    .pairing_complete = diagnostics_pairing_complete,
    .pairing_failed = diagnostics_pairing_failed,
};

static int diagnostics_init(void) {
    int err = bt_conn_auth_info_cb_register(&diagnostics_auth_callbacks);
    LOG_INF("diagnostics ready result=%d; role 0=split, role 1=host", err);
    return err;
}

SYS_INIT(diagnostics_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
