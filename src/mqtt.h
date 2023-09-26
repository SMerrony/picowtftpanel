/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 */

#ifndef MQTT_H
#define MQTT_H

#include "lwip/apps/mqtt.h"

// Define ONE of the following (see also info_items.c)
// #define CLOCK1
#define CLOCK2
// #define INFOPANEL1

#ifdef CLOCK1
    #define MQTT_CLIENT_ID "PicowClock1"
    #define MQTT_CONTROL_TOPIC "rgbmatrix/control4"
#endif
#ifdef CLOCK2
    #define MQTT_CLIENT_ID "PicowClock2"
    #define MQTT_CONTROL_TOPIC "rgbmatrix/control2"
#endif
#ifdef INFOPANEL1
    #define MQTT_CLIENT_ID "PicowPanel1"
    #define MQTT_CONTROL_TOPIC "rgbmatrix/control1"
#endif

#define BROKER_HOST "192.168.1.10"
#define BROKER_PORT 1883
#define BROKER_KEEPALIVE 60
#define TOPIC          "rgbmatrix/#"         // <== This is what we subscribe to
#define URGENT_TOPIC   "rgbmatrix/urgent"
#define TEMP_TOPIC     "rgbmatrix/Pauls_Studio/temperature"
#define HUMIDITY_TOPIC "rgbmatrix/Pauls_Studio/humidity"

// User topics are matched to IDs 0 .. n
#define ID_UNKNOWN -1
#define ID_CONTROL 998
#define ID_URGENT 999

void mqtt_setup_client();
void mqtt_connect();
bool mqtt_connected();
void publish_sensors(float temp, float hum);

#endif