/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "pico/sync.h"

#include "image.h"
#include "lcd.h"

rgb_t string2rgb(const char *s);

void graphics_init(mutex_t * image_mutex);
void clear_to_black (image_t img);
// void show_3x5_char  (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y);
// void show_5x7_char  (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y);
// void show_6x10_char (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y);
// void show_10x14_char (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y);
// void show_40x56_char (image_t img, unsigned char c, rgb_t fg, uint16_t x, uint16_t y);
void show_3x5_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg);
void show_5x7_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg);
void show_6x10_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg);
void show_10x14_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg);
void show_40x56_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg);

#endif // DISPLAY_H