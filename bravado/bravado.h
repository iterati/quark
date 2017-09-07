#ifndef __BRAVADO_H
#define __BRAVADO_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c1.h"

#define _XTAL_FREQ  16000000

#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define CONSTRAIN(x, n, m) (((x) < (n)) ? (n) : (((x) > (m)) ? (m) : (x)))

#define STATE_OFF 0
#define STATE_ON  1

#define ACCEL_ADDR      0x1C
#define ACCEL_READY     PORTAbits.RA5
#define BUTTON_PRESSED  (PORTCbits.RC2 == 0)
#define NUM_MODES         8
#define ACCEL_BINS        100
#define ACCEL_THRESH_START  65536

static const ACCEL_THRESH_DS[4] = {
    3320,   // 3.5
    3990,   // 4.0
    4670,   // 4.5
    5350,   // 5.0
};

static const ACCEL_THRESH_DDS[4] = {
    80,     // 3.5
    115,    // 4.0
    160,    // 4.5
    210,    // 5.0
};

#define MMA_CTRL_REG1     0x2A
#define MMA_CTRL_REG1_    0b00100001 // 50Hz Active
#define MMA_CTRL_REG2     0x2B
#define MMA_CTRL_REG2_    0b00011011 // Low power active and sleep
#define MMA_CTRL_REG3     0x2C
#define MMA_CTRL_REG3_    0b00000010 // Interrupt active high
#define MMA_CTRL_REG4     0x2D
#define MMA_CTRL_REG4_    0b00000001 // Data ready interrupt enabled
#define MMA_CTRL_REG5     0x2E
#define MMA_CTRL_REG5_    0b00000001 // Data ready interrupt on INT1
#define MMA_XYZ_DATA_CFG  0x0E
#define MMA_XYZ_DATA_CFG_ 0b00000010 // +- 8G range

#define LED_R     CCPR3L
#define LED_B     CCPR2L
#define LED_G     CCPR1L

#define _X        0
#define _Y        1
#define _Z        2
#define AXIS_X    axis[_X]
#define AXIS_Y    axis[_Y]
#define AXIS_Z    axis[_Z]
#define AXIS_X2   axis2[_X]
#define AXIS_Y2   axis2[_Y]
#define AXIS_Z2   axis2[_Z]

typedef struct RGB {
    uint8_t r, g, b;
} RGB;

typedef struct HSV {
    uint8_t h, s, v;
} HSV;

const uint8_t VALS_R[17] = {0, 1, 3, 7, 12, 19, 27, 37, 48, 61, 75, 91, 108, 127, 147, 169, 192};
const uint8_t VALS_G[17] = {0, 1, 3, 7, 13, 20, 29, 40, 52, 66, 81, 98, 117, 137, 159, 183, 208};
const uint8_t VALS_B[17] = {0, 1, 4, 8, 14, 22, 32, 43, 56, 71, 88, 106, 126, 148, 172, 197, 224};
const uint8_t WHITE_R = 21;
const uint8_t WHITE_G = 23;
const uint8_t WHITE_B = 25;

/* const uint8_t CURVE64[9] = {0, 1, 4, 9, 16, 25, 36, 49, 64}; */
/* const uint8_t CURVE64[17] = {0, 1, 1, 2, 4, 6, 9, 12, 16, 20, 25, 30, 36, 42, 49, 56, 64}; */
const uint8_t CURVE64[17] = {0, 1, 2, 3, 4, 6, 9, 12, 16, 20, 25, 30, 36, 42, 49, 56, 64};

#endif
