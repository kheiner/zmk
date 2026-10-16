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
#include <zmk/event_manager.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

enum zmk_battery_charge_state zmk_battery_charge_state(void) {
    return (zmk_usb_get_conn_state() != ZMK_USB_CONN_NONE)
               ? ZMK_BATTERY_CHARGE_STATE_CHARGING
               : ZMK_BATTERY_CHARGE_STATE_UNKNOWN;
}

// Charge state changes are otherwise only seen by the 60 s battery poll, which stops while the
// keyboard is idle; re-run the update so hosts learn the new state when the bus power flips.
static int battery_usb_charge_proxy_listener(const zmk_event_t *eh) {
    const struct zmk_usb_conn_state_changed *ev = as_zmk_usb_conn_state_changed(eh);
    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_battery_update_now();

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(battery_usb_charge_proxy, battery_usb_charge_proxy_listener);

ZMK_SUBSCRIPTION(battery_usb_charge_proxy, zmk_usb_conn_state_changed);
