#ifndef __QUARK_H
#define __QUARK_H

#include <Arduino.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define LIMITER_US        32000

#define ACC_THRESH_START  65536
static const uint16_t ACC_THRESH_D[8] = {6288, 7364, 8448, 10640, 12864, 15120, 19728, 24464};
static const uint16_t ACC_THRESH_DD[8] = {288, 392, 512, 800, 1152, 1568, 2592, 3872};

#define PIN_R             9     // Red pin - timer 0
#define PIN_G             6     // Green pin - timer 1
#define PIN_B             5     // Blue pin - timer 1
#define PIN_BUTTON        2     // Pin for the button
#define PIN_LDO           A3    // Low voltage dropoff pin
#define ADDR_LOCKED       0x0002

#define STATE_OFF         0
#define STATE_LOCKED      1
#define STATE_ON          2
#define STATE_EDIT_MODE   3
#define STATE_EDIT_COLOR  4

#define SER_VERSION       0x42
#define SER_HANDSHAKE     0xe0
#define SER_DISCONNECT    0xff
#define SER_INIT          0x12
#define SER_VIEW_MODE     0xc0
#define SER_VIEW_COLOR    0xcc
#define SER_EDIT_MODE     0xe0
#define SER_EDIT_COLOR    0xec

#define ACCEL_ADDR        0x1D  // I2C address of accelerometer
#define SCL_PIN           A5    // Clock pin
#define SDA_PIN           A4    // Data pin
#define I2CADC_H          315   // Analog read high threshold
#define I2CADC_L          150   // Analog read low threshold

#define NOP __asm__("nop\n\t")
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define SEG mode.segment_num
#define MSEG(x) mode.segments[SEG].x

#define COLOR(x) MSEG(palette)[(x)]
#define CSTATE(x) cstates[SEG][(x)]

// change modes
#define CHANGE_OFF            0b000 // 0
#define CHANGE_THROUGH        0b001 // 1
#define CHANGE_THROUGH_ALT    0b101 // 5
#define CHANGE_OVER           0b011 // 3
#define CHANGE_OVER_ALT       0b111 // 7
// color modes
#define COLOR_MODE_BASIC      0
#define COLOR_MODE_GROUP      1
#define COLOR_MODE_REVERSE    2
#define COLOR_MODE_SHIFT      3
// cmod2 for group and reverse is the drop setting
#define CMOD2_DROP_ONE        0
#define CMOD2_DROP_ALL        1
// cmod 1 for shift if rot dir
#define CMOD1_ROT_C           0b00 // 0
#define CMOD1_ROT_CC          0b01 // 1
#define CMOD1_ROT_C_ALT       0b10 // 2
#define CMOD1_ROT_CC_ALT      0b11 // 3
// cmod 2 for shift is effect dir
#define CMOD2_EFFECT_TO       0b00 // 0
#define CMOD2_EFFECT_FROM     0b01 // 1
#define CMOD2_EFFECT_TO_ALT   0b10 // 2
#define CMOD2_EFFECT_FROM_ALT 0b11 // 3
// dim mode values
#define DIM_MODE_OFF          0
#define DIM_MODE_FADE_IN      1
#define DIM_MODE_FADE_OUT     2
#define DIM_MODE_PULSE        3
// color types
#define COLOR_STATIC          0
#define COLOR_COMPOUND        1
#define COLOR_REACTIVE        2
#define COLOR_MORPH           3
#define COLOR_STREAK          4
#define COLOR_ROTATE          5
#define COLOR_RAINBOW         6

typedef union RGB {
  struct {
    uint8_t r, g, b;
  };
  uint8_t   data[3];
} RGB;

typedef union HSV {
  struct {
    uint8_t h, s, v;
  };
  uint8_t   data[3];
} HSV;

typedef union Color { // 24
  struct {
    uint8_t   type;
    uint8_t   numc;
    uint8_t   dir;
    uint8_t   timer_v[4];
    HSV       colors[4];

    uint8_t   timer;
    uint8_t   tick;
    uint8_t   repeat;
    uint8_t   counter;
    uint8_t   step;
  };
  uint8_t     data[24];
} Color;

typedef union Segment { //  248
  struct {
    uint8_t   numc;
    uint8_t   change_mode;
    uint8_t   color_mode;
    uint8_t   dim_mode;
    uint8_t   cmod1;            // pick or rotation_dir
    uint8_t   cmod2;            // skip_all or effect_dir
    uint8_t   strobe_v[4];
    uint8_t   strobe_end_v[4];
    uint8_t   blank_v[4];
    uint8_t   blank_end_v[4];
    uint8_t   gap_v[4];
    uint8_t   strobe_count_v[4];
    uint8_t   repeat_count_v[4];
    uint8_t   change_count_v[4];
    uint8_t   shift_delta_v[4];   // 42

    uint8_t   cidx;
    uint8_t   repeat_num;
    uint8_t   change_num;
    uint8_t   flip_rot;
    uint8_t   flip_effect;
    uint8_t   strobe;
    uint8_t   strobe_end;
    uint8_t   blank;
    uint8_t   blank_end;
    uint8_t   gap;
    uint8_t   strobe_count;
    uint8_t   repeat_count;
    uint8_t   change_count;
    uint8_t   shift_delta;      // 14

    Color     palette[8];       // 8 * 24 = 192
  };
  uint8_t     data[248];
} Segment;

typedef union Mode { // 1024
  struct {
    uint8_t   nums;
    uint8_t   numt;
    uint8_t   falloff_v[4];
    uint8_t   trigger_v[4];
    uint8_t   thresh[4];
    uint8_t   jump[3];                // 17

    uint8_t   velocity;
    uint8_t   falloff;
    uint8_t   trigger;
    uint8_t   iv_s;
    uint8_t   iv_e;
    uint8_t   iv_f;
    uint8_t   segment_num;
    uint8_t   strobe_num;
    uint8_t   part;
    uint8_t   trip;
    uint8_t   seg_len;

    uint8_t   _[4];

    Segment   segments[NUM_SEGMENTS]; // 248 each
  };
  uint8_t     data[MODE_SIZE];
} Mode;

const uint8_t VALS_R[9] = {0, 3, 12, 27, 48, 75, 108, 147, 192};
const uint8_t VALS_G[9] = {0, 3, 13, 29, 52, 81, 117, 159, 208};
const uint8_t VALS_B[9] = {0, 3, 14, 31, 56, 87, 126, 171, 224};
const uint16_t FRAC8U[8] = {0x0000, 0x0400, 0x1000, 0x2400, 0x4000, 0x6400, 0x9000, 0xc400};
const uint16_t FRAC8D[8] = {0x0000, 0x3C00, 0x7000, 0x9C00, 0xC000, 0xDC00, 0xF000, 0xFC00};
const uint16_t FRAC8C[8] = {0x0000, 0x0800, 0x2000, 0x4800, 0x8000, 0xB800, 0xE000, 0xF800};

#endif
