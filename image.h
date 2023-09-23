/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

#define LCD_HEIGHT 320
#define LCD_WIDTH  480

// typedef struct {
//     uint8_t r;
//     uint8_t g;
//     uint8_t b;
// } rgb_t;

typedef struct {
    uint8_t r : 2;
    uint8_t g : 2;
    uint8_t b : 2;
} rgb_t;


extern const rgb_t BLACK, RED, GREEN, BLUE, WHITE, CYAN, MAGENTA, YELLOW;

// typedef uint32_t image_t[WIDTH][HEIGHT];
typedef rgb_t image_t[LCD_WIDTH][LCD_HEIGHT];

#endif