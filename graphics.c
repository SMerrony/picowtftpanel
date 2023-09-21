/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

// #include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "font_3x5.h"
#include "font_5x7.h"
#include "font_led.h"

const rgb_t BLACK   = {0, 0, 0};
const rgb_t RED   = {3, 0, 0};
const rgb_t GREEN = {0, 3, 0};
const rgb_t BLUE  = {0, 0, 3};
const rgb_t WHITE = {3, 3, 3};
const rgb_t CYAN = {0, 3, 3};
const rgb_t MAGENTA = {3, 0, 3};
const rgb_t YELLOW = {3, 3, 0};

rgb_t string2rgb(char *s) {
    if (strcmp("BLACK", s) == 0) return BLACK;
    if (strcmp("RED", s) == 0) return RED;
    if (strcmp("GREEN", s) == 0) return GREEN;
    if (strcmp("BLUE", s) == 0) return BLUE;
    if (strcmp("WHITE", s) == 0) return WHITE;
    if (strcmp("CYAN", s) == 0) return CYAN;
    if (strcmp("MAGENTA", s) == 0) return MAGENTA;
    if (strcmp("YELLOW", s) == 0) return YELLOW;
    // failsafe
    return WHITE;
}

void show_line (image_t img, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, rgb_t col) {
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        img[x0][y0] = col;
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void clear_to_black (image_t img) {
    for (int x = 0; x < LCD_WIDTH; ++x) {
        for (int y = 0; y < LCD_HEIGHT; ++y) {
            img[x][y] = BLACK;
        }
    }
}

void show_sisd_char (image_t img, unsigned char c, rgb_t fg, uint16_t x, uint16_t y, 
                     uint16_t half_width, uint16_t half_height) {
    if ((x + (2 * half_width) < LCD_WIDTH) && (y + (2 * half_height) < LCD_HEIGHT)) {
        if ((LED_16_SEG[c] & 0b0000000000000000) != 0) {
            show_line(img, x, y, x + half_width, y, fg);
        }
    }
}

void show_3x5_char (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y) {
    uint16_t col = 0;
    // uint32_t fg_gbr = rgb2bgr32(fg);
    // uint32_t bg_gbr = rgb2bgr32(bg);
    if ((x < (LCD_WIDTH - 3)) && (y < (LCD_HEIGHT - 4))) { // Don't draw outside bounds
        while (col < 3) {
            img[x + col][y + 4] = ((font_3x5[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + col][y + 3] = ((font_3x5[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + col][y + 2] = ((font_3x5[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + col][y + 1] = ((font_3x5[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + col][y]     = ((font_3x5[c][col] & 0b00000010) != 0) ? fg : bg;
            ++col;
        }
    }
}

void show_5x7_char (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y) {
    uint16_t col = 0;
    if ((x < (LCD_WIDTH - 5)) && (y < (LCD_HEIGHT - 7))) { // Don't draw outside bounds
        while (col < 5) {
            img[x + col][y + 6] = ((font_5x7[c][col] & 0b01000000) != 0) ? fg : bg;
            img[x + col][y + 5] = ((font_5x7[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + col][y + 4] = ((font_5x7[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + col][y + 3] = ((font_5x7[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + col][y + 2] = ((font_5x7[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + col][y + 1] = ((font_5x7[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + col][y]     = ((font_5x7[c][col] & 0b00000001) != 0) ? fg : bg;
            ++col;
        }
    }
}

void show_6x10_char (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y) {
    uint16_t col = 0;
    if ((x < (LCD_WIDTH - 6)) && (y < (LCD_HEIGHT - 10))) { // Don't draw outside bounds
        while (col < 3) {
            img[x + (col * 2)][y + 8]     = ((font_3x5[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 9]     = ((font_3x5[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 8] = ((font_3x5[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + (1 + col * 2)][y + 9] = ((font_3x5[c][col] & 0b00100000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 6]     = ((font_3x5[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 7]     = ((font_3x5[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 6] = ((font_3x5[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + (1 + col * 2)][y + 7] = ((font_3x5[c][col] & 0b00010000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 4]     = ((font_3x5[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 5]     = ((font_3x5[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 4] = ((font_3x5[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + (1 + col * 2)][y + 5] = ((font_3x5[c][col] & 0b00001000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 2]     = ((font_3x5[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + (col * 2)][y + 3]     = ((font_3x5[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 2] = ((font_3x5[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + (1 + col * 2)][y + 3] = ((font_3x5[c][col] & 0b00000100) != 0) ? fg : bg;

            img[x + (col * 2)][y]         = ((font_3x5[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + (col * 2)][y + 1]     = ((font_3x5[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y]     = ((font_3x5[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 1] = ((font_3x5[c][col] & 0b00000010) != 0) ? fg : bg;
            ++col;
        }
    }
}

void show_10x14_char (image_t img, unsigned char c, rgb_t fg, rgb_t bg, uint16_t x, uint16_t y) {
    uint16_t col = 0;
    if ((x < (LCD_WIDTH - 10)) && (y < (LCD_HEIGHT - 14))) { // Don't draw outside bounds
        while (col < 5) {
            img[x + (col * 2)][y + 12] = ((font_5x7[c][col] & 0b01000000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 13] = ((font_5x7[c][col] & 0b01000000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 12] = ((font_5x7[c][col] & 0b01000000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 13] = ((font_5x7[c][col] & 0b01000000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 10] = ((font_5x7[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 11] = ((font_5x7[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 10] = ((font_5x7[c][col] & 0b00100000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 11] = ((font_5x7[c][col] & 0b00100000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 8] = ((font_5x7[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 9] = ((font_5x7[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 8] = ((font_5x7[c][col] & 0b00010000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 9] = ((font_5x7[c][col] & 0b00010000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 6] = ((font_5x7[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + (col * 2)][y + 7] = ((font_5x7[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 6] = ((font_5x7[c][col] & 0b00001000) != 0) ? fg : bg;
            img[x + (1 + col * 2)][y + 7] = ((font_5x7[c][col] & 0b00001000) != 0) ? fg : bg;

            img[x + (col * 2)][y + 4] = ((font_5x7[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + (col * 2)][y + 5] = ((font_5x7[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 4] = ((font_5x7[c][col] & 0b00000100) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 5] = ((font_5x7[c][col] & 0b00000100) != 0) ? fg : bg;

            img[x + (col * 2)][y + 2] = ((font_5x7[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + (col * 2)][y + 3] = ((font_5x7[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 2] = ((font_5x7[c][col] & 0b00000010) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 3] = ((font_5x7[c][col] & 0b00000010) != 0) ? fg : bg;

            img[x + (col * 2)][y]         = ((font_5x7[c][col] & 0b00000001) != 0) ? fg : bg;
            img[x + (col * 2)][y + 1]     = ((font_5x7[c][col] & 0b00000001) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y]         = ((font_5x7[c][col] & 0b00000001) != 0) ? fg : bg;
            img[x + 1 + (col * 2)][y + 1]     = ((font_5x7[c][col] & 0b00000001) != 0) ? fg : bg;

            ++col;
        }
    }
}

void show_block (image_t img, uint16_t x, uint16_t y, int width, int height, rgb_t col) {
    for (int ix = x; ix < x + width; ++ix) {
        for (int iy = y; iy < y + height; ++iy) {
            if ((iy < LCD_HEIGHT) && (ix < LCD_WIDTH)) img[ix][iy] = col;
        }
    }
}

void show_40x56_char (image_t img, unsigned char c, rgb_t fg, uint16_t x, uint16_t y) {
    if ((x < (LCD_WIDTH - 40)) && (y < (LCD_HEIGHT - 56))) { // Don't draw outside bounds
        for (int font_row = 0; font_row < 7; font_row++) {
            for (int font_col = 0; font_col < 5; font_col++) {
                if ((font_5x7[c][font_col] & (1 << font_row)) != 0) {
                    show_block(img, x + (font_col * 8), y + (font_row * 8), 8, 8, fg);
                }
            }
        }
    }
}

void show_3x5_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg) {
    // clear background
    int w = strlen(msg) * 4; // 1 pixel gap between chars
    show_block(img, x, y, w, 5, bg);
    for (unsigned int ix = 0; ix < strlen(msg); ++ix) {
        show_3x5_char(img, msg[ix], fg, bg, x + (ix * 4), y);
    }
}

void show_5x7_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg) {
    // clear background
    int w = strlen(msg) * 6; // 1 pixel gap between chars
    show_block(img, x, y, w, 7, bg);
    for (unsigned int ix = 0; ix < strlen(msg); ++ix) {
        show_5x7_char(img, msg[ix], fg, bg, x + (ix * 6), y);
    }
}

void show_6x10_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg) {
    // clear background
    int w = strlen(msg) * 8; // 2 pixel gap between chars
    show_block(img, x, y, w, 10, bg);
    for (unsigned int ix = 0; ix < strlen(msg); ++ix) {
        show_6x10_char(img, msg[ix], fg, bg, x + (ix * 8), y);
    }
}

void show_10x14_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg) {
    // clear background
    int w = strlen(msg) * 14; // 4 pixel gap between chars
    show_block(img, x, y, w, 14, bg);
    for (unsigned int ix = 0; ix < strlen(msg); ++ix) {
        show_10x14_char(img, msg[ix], fg, bg, x + (ix * 14), y);
    }
}

void show_40x56_string (image_t img, char msg[], uint16_t x, uint16_t y, rgb_t fg, rgb_t bg) {
    // clear background
    int w = strlen(msg) * 46; // 6 pixel gap between chars
    // printf("DEBUG: Msg: >>%s<< w: %d\n", msg, w);
    show_block(img, x, y, w, 56, bg);
    for (unsigned int ix = 0; ix < strlen(msg); ++ix) {
        show_40x56_char(img, msg[ix], fg, x + (ix * 46), y);
    }
}

void dim (image_t img, int div) {
    rgb_t rgb;
    for (int x = 0; x < LCD_WIDTH; ++x) {
        for (int y = 0; y < LCD_HEIGHT; ++y) {
            rgb = img[x][y];
            rgb.r /= div;
            rgb.g /= div;
            rgb.b /= div;
            img[x][y] = rgb;
        }
    }
}