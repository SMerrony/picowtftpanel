/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: MIT
 * 
 * This code borrows heavily from the LCD driver in https://github.com/ncrawforth/VT2040
 */

#include <stdio.h>
#include <stdlib.h>

#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/sync.h"

#include "lcd.h"
#include "lcd.pio.h"

volatile bool rotate = LCD_ROTATE;
volatile int dirty;
int brightness = BRIGHTNESS_DEFAULT;
bool rotated;
image_t disp_image;
mutex_t * img_mutex;

void lcd_pio_init() {

  uint offset = pio_add_program(LCD_PIO, &lcd_program);
  pio_sm_config c = lcd_program_get_default_config(offset);
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
  sm_config_set_out_shift(&c, false, true, 8);

  pio_gpio_init(LCD_PIO, LCD_PIN_MOSI);
  pio_sm_set_consecutive_pindirs(LCD_PIO, LCD_PIO_SM, LCD_PIN_MOSI, 1, true);
  sm_config_set_out_pins(&c, LCD_PIN_MOSI, 1);

  pio_gpio_init(LCD_PIO, LCD_PIN_CLK);
  pio_sm_set_consecutive_pindirs(LCD_PIO, LCD_PIO_SM, LCD_PIN_CLK, 1, true);
  sm_config_set_sideset_pins(&c, LCD_PIN_CLK);

  pio_sm_init(LCD_PIO, LCD_PIO_SM, offset, &c);
  pio_sm_set_enabled(LCD_PIO, LCD_PIO_SM, true);
}

void lcd_pio_set_dc(bool dc) {
  // Wait until the PIO stalls (meaning it has finished sending)
  uint32_t sm_stall_mask = 1u << (LCD_PIO_SM + PIO_FDEBUG_TXSTALL_LSB);
  LCD_PIO->fdebug = sm_stall_mask;
  while (!(LCD_PIO->fdebug & sm_stall_mask)) tight_loop_contents();
  // Set the Data/Cmd pin (0 = Cmd, 1 = Data)
  gpio_put(LCD_PIN_DC, dc);
}

void lcd_pio_put(uint8_t byte) {
  while (pio_sm_is_tx_fifo_full(LCD_PIO, LCD_PIO_SM)) tight_loop_contents();
  // Add 1 byte of data to the FIFO
  *(volatile uint8_t*)&LCD_PIO->txf[LCD_PIO_SM] = byte;
}

void lcd_off() {
  brightness = 0;
  pwm_set_gpio_level(LCD_PIN_LED, 0);
}

void lcd_on() {
  lcd_pio_put(CMD_SLEEP_OUT);
  lcd_pio_put(CMD_DISPLAY_ON); 
  brightness = BRIGHTNESS_DEFAULT;
  pwm_set_gpio_level(LCD_PIN_LED, BRIGHTNESS_DEFAULT * BRIGHTNESS_DEFAULT);
}

void core1_main() {

  lcd_pio_init();

  // Hold the CS pin low
  gpio_init(LCD_PIN_CS);
  gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
  gpio_put(LCD_PIN_CS, 0);

  // Hold the RST pin high
  gpio_init(LCD_PIN_RST);
  gpio_set_dir(LCD_PIN_RST, GPIO_OUT);
  gpio_put(LCD_PIN_RST, 1);

  // Initialise the DC pin
  gpio_init(LCD_PIN_DC);
  gpio_set_dir(LCD_PIN_DC, GPIO_OUT);

  // Wait for the LCD to settle
  busy_wait_ms(100);
  
  // Initialise the LCD
  lcd_pio_set_dc(0);
  pio_sm_set_clkdiv(LCD_PIO, LCD_PIO_SM, LCD_PIO_CLKDIV_CMD);
  lcd_pio_put(CMD_SLEEP_OUT); 
  lcd_on(); 

  // Clear the LCD
  lcd_pio_set_dc(0);
  lcd_pio_put(CMD_MEMORY_WRITE); // Cmd: Memory Write
  lcd_pio_set_dc(1);
  for (int i = 0; i < (LCD_WIDTH * LCD_HEIGHT * 3); i++) {
    lcd_pio_put(0); // Data: 0
  }

  // Wait a moment for screen to refresh
  busy_wait_ms(100);

  // Turn on the LCD backlight
  gpio_set_function(LCD_PIN_LED, GPIO_FUNC_PWM);
  pwm_set_clkdiv(pwm_gpio_to_slice_num(LCD_PIN_LED), 10.f);
  pwm_set_wrap(pwm_gpio_to_slice_num(LCD_PIN_LED), BRIGHTNESS_MAX * BRIGHTNESS_MAX);
  pwm_set_gpio_level(LCD_PIN_LED, brightness * brightness);
  pwm_set_enabled(pwm_gpio_to_slice_num(LCD_PIN_LED), 1);

  while (1) {
    while (dirty == 0) busy_wait_ms(UPDATE_PERIOD_MS);
    dirty--;

    // if (rotate) {
    //   rotate = false;
    //   lcd_pio_set_dc(0);
    //   pio_sm_set_clkdiv(LCD_PIO, LCD_PIO_SM, LCD_PIO_CLKDIV_CMD);
    //   lcd_pio_put(CMD_MEMORY_ACCESS_CONTROL);
    //   lcd_pio_set_dc(1);
    //   if (rotated) {
    //     lcd_pio_put(0x0);
    //     rotated = 0;
    //   } else {
    //     lcd_pio_put(0xc0);
    //     rotated = 1;
    //   }
    // }

    mutex_enter_blocking(img_mutex); // <<<<<<<<<<<<<  Lock image <<<<<<<<<<<<<<
    lcd_pio_set_dc(0);
    lcd_pio_put(CMD_MEMORY_WRITE); // Cmd: Memory Write
    lcd_pio_set_dc(1);
    for (int x = 0; x < LCD_WIDTH; x++) {
      for (int y = 0; y < LCD_HEIGHT; y++) {
          lcd_pio_put(disp_image[x][y].b<<7);
          lcd_pio_put(disp_image[x][y].g<<7);
          lcd_pio_put(disp_image[x][y].r<<7);
      }
    }
    mutex_exit(img_mutex);    // <<<<<<<<<<< Unlock image <<<<<<<<<<<<<<<
  }
}

image_t * lcd_init(mutex_t *mtx) {
  img_mutex = mtx;
  multicore_launch_core1(core1_main);
  return &disp_image;
}

void lcd_invalidate() {
  dirty = 2;
}

void lcd_brighten() {
  if (brightness < BRIGHTNESS_MAX) brightness++;
  pwm_set_gpio_level(LCD_PIN_LED, brightness * brightness);
}

void lcd_darken() {
  if (brightness > 1) brightness--;
  pwm_set_gpio_level(LCD_PIN_LED, brightness * brightness);
}

void lcd_rotate() {
  rotate = true;
  lcd_invalidate();
}
