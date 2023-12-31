/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 */

#include "info_items.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "graphics.h"
#include "lcd.h"
#include "mqtt.h"

static image_t *ii_image;
static bool showing_urgent;
static char urgent_msg[MAX_URGENT_CHARS];

#ifdef CLOCK1
    const int INFO_ITEM_COUNT = 4;
    info_item_t info_items[] = {
        {"rgbmatrix/time_hhmmss", "", "", 1, 0, "YELLOW", "BLACK", "3x5", 2},
        {"rgbmatrix/time_date", "", "", 2, 12, "MAGENTA", "BLACK", "5x7", 1},
        {"rgbmatrix/music_temp", "", "C", 0, 22, "CYAN", "BLACK", "3x5", 2},
        {"rgbmatrix/music_hum", "", "%", 42, 22, "BLUE", "BLACK", "3x5", 2}
    };
    info_item_t urgent_item = {URGENT_TOPIC, "", "", 0, 22, "RED", "BLACK", "3x5", 2};
#endif
#ifdef CLOCK2
    const int INFO_ITEM_COUNT = 4;
    const info_item_t info_items[] = {
        {"rgbmatrix/time_hhmm", "", "", 134, 10, "YELLOW", "BLACK", "5x7", 4},
        // {"rgbmatrix/time_hhmmss", "", "", 100, 8, "YELLOW", "BLACK", "5x7", 4},
        {"rgbmatrix/time_date", "", "", 10, 126, "MAGENTA", "BLACK", "5x7", 4},
        {"rgbmatrix/Pauls_Studio/temperature", "", "C", 8, 250, "CYAN", "BLACK", "5x7", 4},
        {"rgbmatrix/outside_temp", "", "C", 330, 250, "GREEN", "BLACK", "5x7", 4}
    };
    const info_item_t urgent_item = {URGENT_TOPIC, "", "", 0, 250, "RED", "BLACK", "5x7", 4};
#endif
#ifdef INFOPANEL1
    const int INFO_ITEM_COUNT = 5;
    info_item_t info_items[] = {
        {"rgbmatrix/time_hhmmss", "", "", 1, 0, "YELLOW", "BLACK", "3x5", 2},
        {"rgbmatrix/time_date", "", "", 2, 12, "MAGENTA", "BLACK", "5x7", 1},
        {"rgbmatrix/office_temp", "", "C", 9, 22, "CYAN", "BLACK", "3x5", 2},
        {"rgbmatrix/outside_temp", "/ ", "", 42, 22, "BLUE", "BLACK", "3x5", 2},
        {"rgbmatix/gbpeur", "", "", 8, 39, "MAGENTA", "BLACK", "3x5", 1}
    };
    info_item_t urgent_item = {URGENT_TOPIC, "", "", 0, 44, "RED", "BLACK", "5x7", 2};
#endif

void info_setup(image_t *image) {
    ii_image = image;
    showing_urgent = false;
}

uint32_t getTotalHeap(void) {
   extern char __StackLimit, __bss_end__;
   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap(void) {
   struct mallinfo m = mallinfo();
   return getTotalHeap() - m.uordblks;
}

void show_urgent() {
    if (showing_urgent) {
        show_40x56_string(*ii_image, 
                        urgent_msg, 
                        urgent_item.x, 
                        urgent_item.y, 
                        string2rgb(urgent_item.fg), 
                        string2rgb(urgent_item.bg)
                        );
        lcd_invalidate();
    }
}

/* hide_urgent displays the urgent message black-on-black
   It is intended to be used for the blink effect. */
void hide_urgent() {
    if (showing_urgent) {
        show_40x56_string(*ii_image, 
                urgent_msg, 
                urgent_item.x, 
                urgent_item.y, 
                BLACK, 
                BLACK
                );
        lcd_invalidate();
    }
}

void show_data(int id, const char *data, int len) {
    if (id < INFO_ITEM_COUNT) { // handle msg on a subscribed topic
        char info[40] = "";
        if (strlen(info_items[id].prefix) > 0) strcat(info, info_items[id].prefix);
        strncat(info, data, len);
        if (strlen(info_items[id].suffix) > 0) strcat(info, info_items[id].suffix);
        if (strcmp(info_items[id].font, "3x5") == 0) {
            if (info_items[id].scale == 1) {
                show_3x5_string(*ii_image, 
                                info, 
                                info_items[id].x, 
                                info_items[id].y, 
                                string2rgb(info_items[id].fg), 
                                string2rgb(info_items[id].bg)
                            );
            } else {
                show_6x10_string(*ii_image, 
                                info, 
                                info_items[id].x, 
                                info_items[id].y, 
                                string2rgb(info_items[id].fg), 
                                string2rgb(info_items[id].bg)
                                );
            }
        } else {
            switch (info_items[id].scale) {
                case 1:
                    show_5x7_string(*ii_image, 
                                info, 
                                info_items[id].x, 
                                info_items[id].y, 
                                string2rgb(info_items[id].fg), 
                                string2rgb(info_items[id].bg)
                                );
                    break;
                case 2:
                    show_10x14_string(*ii_image, 
                                info, 
                                info_items[id].x, 
                                info_items[id].y, 
                                string2rgb(info_items[id].fg), 
                                string2rgb(info_items[id].bg)
                                );
                    break;
                case 4:
                    show_40x56_string(*ii_image, 
                                info, 
                                info_items[id].x, 
                                info_items[id].y, 
                                string2rgb(info_items[id].fg), 
                                string2rgb(info_items[id].bg)
                                );
            }
        }
        lcd_invalidate();
        return;
    } 
    if (id == ID_CONTROL) {
        if (strcmp(data, "Off") == 0) {
            // set_blank_display(true);
            lcd_off();
        }
        if (strcmp(data, "On") == 0) {
            // set_blank_display(false);
            lcd_on();
        }
        if (strcmp(data, "Darker") == 0) {
            lcd_darken();
        }
        if (strcmp(data, "Lighter") == 0) {
            lcd_brighten();
        }
        if (strcmp(data, "Memory") == 0) {
            printf("INFO: Free memory: %lu\n", getFreeHeap());
        }
        return;
    }
    if (id == ID_URGENT) {
        if (strlen(data) > 0) {
            showing_urgent = true;
            strncpy(urgent_msg, data, MAX_URGENT_CHARS);
            show_urgent();
        } else {
            hide_urgent();            
            showing_urgent = false;
        }
        return;
    }
    
    // shouldn't get here
    printf("WARNING: Unexpected info item, not handled (in info_items.c)\n");
}