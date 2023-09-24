/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include <pico/stdlib.h>
#include "pico/sync.h"
#include "pico/time.h"

#include "graphics.h"
#include "info_items.h"
#include "lcd.h"
#include "mqtt.h"
#include "pico_dht/dht/include/dht.h"
#include "wifi_config.h"

#define RETRY_MS 5000
#define BLINK_PERIOD_MS 1000
#define WATCHDOG_TIMEOUT_MS  3000 // max is ~4700
#define DHT_SAMPLE_PERIOD 60 // multiply by BLINK_PERIOD_MS to get real period

static image_t * image_ptr;
static mutex_t image_mutex;
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 1; // GPIO1 - physical pin 2

int main() {
    stdio_init_all();
    // busy_wait_ms(RETRY_MS); // DEBUGGING - time to connect terminal

    if (watchdog_caused_reboot()) printf("INFO: Rebooted due to Watchdog timeout\n");

    // set up the AM2302/DHT22 Temp & Hum sensor
    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio1, DATA_PIN, true);

    mutex_init(&image_mutex);

    graphics_init(&image_mutex);

    // fire up the display
    image_ptr = lcd_init(&image_mutex);

    // initialise and connect to WiFi
    while (cyw43_arch_init_with_country(WIFI_COUNTRY) != 0) {
        printf("ERROR: WiFi failed to initialise - will retry in 5s\n");
        busy_wait_ms(RETRY_MS);
    }  
    cyw43_arch_enable_sta_mode();
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT_MS) != 0) {
        printf("ERROR: Wifi failed to connect - will retry in 5s\n");
        busy_wait_ms(RETRY_MS);
    }
    printf("DEBUG: Wifi connected\n");

    /* N.B. The following power management call is critical if you want to achieve smooth
            running w.r.t. MQTT message handling.  Without it, reception appears to stall
            approximately every 3 seconds. */
    cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 20, 1, 1, 1));
        
    info_setup(image_ptr);

    mqtt_setup_client();
    mqtt_connect();     // does not return until connection is established
    printf("DEBUG: Connected to MQTT broker\n");

    watchdog_enable(WATCHDOG_TIMEOUT_MS, true);

    bool flash_toggle = false;
    int dht_counter = 0;
    float humidity = 0.0f;
    float temperature = 0.0f;
    while (true) {
        busy_wait_ms(BLINK_PERIOD_MS);
        flash_toggle = !flash_toggle;
        if (flash_toggle) show_urgent(); else hide_urgent();

        if (mqtt_connected()) watchdog_update();    // feed the watchdog

        ++dht_counter;
        if (dht_counter == DHT_SAMPLE_PERIOD) {
            dht_counter = 0;
            dht_start_measurement(&dht);
            dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature);
            if (result == DHT_RESULT_OK) {
                if (mqtt_connected()) {
                    publish_sensors(temperature, humidity);
                }
            } else {
                printf("WARNING: DHT Sensor read failure\n");
            }
        }
    }
}