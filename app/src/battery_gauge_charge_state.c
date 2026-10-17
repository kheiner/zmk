/*
 * Copyright (c) 2026 kheiner, on top of zmk PR #3458's battery charge status work.
 * SPDX-License-Identifier: MIT
 *
 * Charge state from a fuel gauge's measured average current. Unlike the charger-status
 * pin or the USB-bus proxy, the gauge sees charge flow directly, so tapering current
 * correctly stops claiming "charging" before the cell is full.
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include <zmk/battery.h>

// Averaged current above this magnitude means charge is flowing; anything between the
// thresholds is tapering or resting (treated as discharged-not-charging for reporting).
#define GAUGE_CURRENT_THRESHOLD_MA 3

static const struct device *const gauge = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));

enum zmk_battery_charge_state zmk_battery_charge_state(void) {
    struct sensor_value avg;

    if (!device_is_ready(gauge) || sensor_sample_fetch_chan(gauge, SENSOR_CHAN_GAUGE_AVG_CURRENT) != 0 ||
        sensor_channel_get(gauge, SENSOR_CHAN_GAUGE_AVG_CURRENT, &avg) != 0) {
        return ZMK_BATTERY_CHARGE_STATE_UNKNOWN;
    }

    int ma = avg.val1;

    return (ma <= -GAUGE_CURRENT_THRESHOLD_MA) ? ZMK_BATTERY_CHARGE_STATE_DISCHARGING
           : (ma >= GAUGE_CURRENT_THRESHOLD_MA) ? ZMK_BATTERY_CHARGE_STATE_CHARGING
                                                : ZMK_BATTERY_CHARGE_STATE_UNKNOWN;
}