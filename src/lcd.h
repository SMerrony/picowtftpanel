/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 * 
 * This code borrows heavily from the LCD driver in https://github.com/ncrawforth/VT2040
 */

#ifndef LCD_H
#define LCD_H

#include "pico/sync.h"

#include "graphics.h"
#include "image.h"

#define REFRESH_PERIOD_MS 100

// LCD is rotated 180 degrees
#define LCD_ROTATE false

// Number of backlight brightness steps
#define BRIGHTNESS_MAX 10

// Default backlight brightness
#define BRIGHTNESS_DEFAULT 9

// Select which GPIO pins to use
#define LCD_PIN_CS 8
#define LCD_PIN_RST 9
#define LCD_PIN_DC 10
#define LCD_PIN_MOSI 11
#define LCD_PIN_CLK 12
#define LCD_PIN_LED 13

// Select which PIO and state machine to use
#define LCD_PIO pio0
#define LCD_PIO_SM 0

// Clock divisor to use when sending commands (lower = faster)
// 6.25 is within the datasheet spec
#define LCD_PIO_CLKDIV_CMD 6.25f

// Clock divisor to use when sending pixel data (lower = faster)
// 1.5 is outside spec but works
#define LCD_PIO_CLKDIV_PIXELS 1.5f

// The small subset of ILI9488 commands we are currently using
#define CMD_SLEEP_IN      0x10
#define CMD_SLEEP_OUT     0x11
#define CMD_DISPLAY_OFF   0x28
#define CMD_DISPLAY_ON    0x29
#define CMD_MEMORY_WRITE  0x2C
#define CMD_MEMORY_ACCESS_CONTROL 0X36
#define CMD_PGAMCTRL      0xE0
#define CMD_NGAMCTRL      0xE1

image_t * lcd_init(mutex_t *mtx);
void lcd_off();
void lcd_on();
void lcd_invalidate();
void lcd_brighten();
void lcd_darken();
// void lcd_invert();
void lcd_rotate();

#endif