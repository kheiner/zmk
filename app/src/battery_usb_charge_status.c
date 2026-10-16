/*
 * Copyright (c) 2026 kheiner, on top of zmk PR #3458's battery charge status work.
 * SPDX-License-Identifier: MIT
 *
 * Charge state from USB bus power: charging while connected, unknown otherwise.
 * A proxy for boards without a charger status pin; unlike a real STAT output it
 * keeps saying "charging" after the battery fills.
 */

#include <zephyr/kernel.h>

#include <zmk/battery.h>
#include <zmk/usb.h>

enum zmk_battery_charge_state zmk_battery_charge_state(void) {
    return (zmk_usb_get_conn_state() != ZMK_USB_CONN_NONE)
               ? ZMK_BATTERY_CHARGE_STATE_CHARGING
               : ZMK_BATTERY_CHARGE_STATE_UNKNOWN;
}
