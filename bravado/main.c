// {{{ CONFIG
// CONFIG1
#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINT1    // Power-up default value for COSC bits->HFINTOSC
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; I/O or oscillator function on OSC2
#pragma config CSWEN = ON    // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config FCMEN = ON    // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor is enabled

// CONFIG2
#pragma config MCLRE = ON    // Master Clear Enable bit->MCLR/VPP pin function is MCLR; Weak pull-up enabled
#pragma config PWRTE = OFF    // Power-up Timer Enable bit->PWRT disabled
#pragma config WDTE = SLEEP    // Watchdog Timer Enable bits->WDT enabled while running and disabled in SLEEP/IDLE; SWDTEN is ignored
#pragma config LPBOREN = OFF    // Low-power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON    // Brown-out Reset Enable bits->Brown-out Reset enabled, SBOREN bit ignored
#pragma config BORV = LOW    // Brown-out Reset Voltage selection bit->Brown-out voltage (Vbor) set to 2.45V
#pragma config PPS1WAY = ON    // PPSLOCK bit One-Way Set Enable bit->The PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a Reset
#pragma config DEBUG = OFF    // Debugger enable bit->Background debugger disabled

// CONFIG3
#pragma config WRT = OFF    // User NVM self-write protection bits->Write protection off
#pragma config LVP = ON    // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.

// CONFIG4
#pragma config CP = ON    // User NVM Program Memory Code Protection bit->User NVM code protection disabled
#pragma config CPD = ON    // Data NVM Memory Code Protection bit->Data NVM code protection disabled
// }}}

#include "bravado.h"

// {{{ GLOBALS
RGB led;
uint8_t mode_idx;
uint8_t cstate;
bool frame_locked;

HSV flash_color;
uint16_t flash_trip;
uint8_t flash_ticks;

uint16_t timers[8];
uint16_t counters[8];
uint8_t trip;
uint8_t segm;
uint8_t cidx;
uint8_t numc;

uint8_t p_color;
uint8_t p_color_end;
uint8_t p_blank;
uint8_t p_blank_end;
uint8_t p_gap;
uint8_t p_tcolor;
uint8_t p_tblank;

uint32_t since_press = 0;
bool was_pressed = false;

uint8_t falloff_value = 8;
uint8_t trigger_value = 0;
uint8_t accel_thresh_range = 3;

int16_t axis[3];
uint32_t axis2[3];

bool new_reading;
uint8_t velocity;
uint8_t new_velocity;
uint32_t magnitude;
uint8_t falloff[ACCEL_BINS];
uint8_t trigger[ACCEL_BINS];

uint8_t accel_step = 0;
uint8_t accel_bin;
uint32_t accel_thresh;
uint32_t accel_thresh_dv;
uint32_t accel_thresh_dd;
// }}}

// {{{ INTERP
uint8_t norm64(uint8_t x, uint8_t d) {
    if (x >= d) return 64;
    while (d < 64) { d <<= 1; x <<= 1; }

    uint8_t v = x;
    uint8_t t = d;
    uint8_t r = 0;

    t >>= 1; if (v >= t) { v -= t; r |= 0x20; }
    t >>= 1; if (v >= t) { v -= t; r |= 0x10; }
    t >>= 1; if (v >= t) { v -= t; r |= 0x08; }
    t >>= 1; if (v >= t) { v -= t; r |= 0x04; }
    t >>= 1; if (v >= t) { v -= t; r |= 0x02; }
    t >>= 1; if (v >= t) { v -= t; r |= 0x01; }
    return r;
}

uint8_t interp64(uint8_t s, uint8_t e, uint8_t n) {
    if (n == 0)  return s;
    if (n >= 64) return e;

    uint8_t d = (s < e) ? e - s : s - e;
    uint8_t r = 0;
    uint8_t a;

    a = d & 0x01; d >>= 1; if (n & 0x20) r += d + a;
    a = d & 0x01; d >>= 1; if (n & 0x10) r += d + a; 
    a = d & 0x01; d >>= 1; if (n & 0x08) r += d + a; 
    a = d & 0x01; d >>= 1; if (n & 0x04) r += d + a; 
    a = d & 0x01; d >>= 1; if (n & 0x02) r += d + a; 
    a = d & 0x01; d >>= 1; if (n & 0x01) r += d + a;
    return (s < e) ? s + r : s - r;
}

uint8_t interp(uint8_t s, uint8_t e, uint8_t x, uint8_t d) {
    uint8_t n = norm64(x, d);
    return interp64(s, e, n);
}

uint8_t interp_v(uint8_t s, uint8_t e, uint8_t n, uint8_t m) {
    uint8_t x = (velocity < n) ? n : (velocity > m) ? m : velocity;
    uint8_t d = m - n;
    uint8_t nx = norm64(velocity, d);
    return interp64(s, e, nx);
}
// }}}

// {{{ COLORS
void _hue(uint8_t h, RGB *rgb) {
    if (h >= 48)
        h -= 48;

    if (h < 16) {
        rgb->r = VALS_R[16 - h];
        rgb->g = VALS_G[h];
        rgb->b = 0;
    } else if (h < 32) {
        rgb->r = 0;
        rgb->g = VALS_G[32 - h];
        rgb->b = VALS_B[h - 16];
    } else {
        rgb->r = VALS_R[h - 32];
        rgb->g = 0;
        rgb->b = VALS_B[48 - h];
    }
}

void _sat(uint8_t s, RGB *rgb) {
    if (s > 16)
        s = 16;

    uint8_t nx_up = CURVE64[s];
    uint8_t nx_dn = 64 - CURVE64[16 - s];
    uint8_t nx;

    nx = (rgb->r < WHITE_R) ? nx_up : nx_dn;
    rgb->r = interp64(rgb->r, WHITE_R, nx);

    nx = (rgb->g < WHITE_G) ? nx_up : nx_dn;
    rgb->g = interp64(rgb->g, WHITE_G, nx);

    nx = (rgb->b < WHITE_B) ? nx_up : nx_dn;
    rgb->b = interp64(rgb->b, WHITE_B, nx);
}


void _val(uint8_t v, RGB *rgb) {
    if (v > 16)
        v = 16;

    uint8_t nx = CURVE64[16 - v];

    rgb->r = interp64(0, rgb->r, nx);
    rgb->g = interp64(0, rgb->g, nx);
    rgb->b = interp64(0, rgb->b, nx);
}

void unhsv(HSV *hsv, RGB *rgb) {
    uint8_t h_hi = hsv->h >> 2;
    RGB rgb1, rgb2;
    _hue(h_hi, &rgb1);
    _hue(h_hi + 1, &rgb2);

    uint8_t h_lo = (hsv->h & 0x03) << 4;
    rgb->r = interp64(rgb1.r, rgb2.r, h_lo);
    rgb->g = interp64(rgb1.g, rgb2.g, h_lo);
    rgb->b = interp64(rgb1.b, rgb2.b, h_lo);

    _sat(hsv->s, rgb);
    _val(hsv->v, rgb);
}

void blend(HSV *hsv1, HSV *hsv2, RGB *rgb, uint8_t x, uint8_t d) {
    RGB rgb1, rgb2;
    unhsv(hsv1, &rgb1);
    unhsv(hsv2, &rgb2);

    uint8_t nx = norm64(x, d);
    rgb->r = interp64(rgb1.r, rgb2.r, nx);
    rgb->g = interp64(rgb1.g, rgb2.g, nx);
    rgb->b = interp64(rgb1.b, rgb2.b, nx);
}
// }}}

// {{{ PATTERNS
// {{{ void pattern_darkside(void)
void pattern_darkside(void) {
    // lo -> md = 3 segments, 1 (1.5ms) -> 40 (60ms)
    // md -> hi = 12 segments, 10 (60ms) -> 3/17
    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            if (velocity < 45) {
                p_color = interp_v(3, 120, 20, 45);
                p_blank = 0;
                p_gap = 120;
                numc = 1;
            } else {
                p_color = interp_v(10, 3, 45, 70);
                p_blank = interp_v(0, 17, 45, 70);
                p_gap = interp_v(120, 17, 45, 70);
                numc = 12;
            }
        }

        if (segm == 0) {
            segm = 1;
            trip = (cidx == (numc - 1)) ? p_gap : p_blank;
        } else {
            segm = 0;
            trip = p_color;
            cidx++;
            if (cidx >= numc) cidx = 0;
        }
    }

    uint8_t hue;
    uint8_t max_h = (numc == 1) ? 192 : 16;
    uint8_t itrip = p_color - trip + 1; // 1 -> p_color

    if (segm == 0) {
        hue = interp(max_h, 0, itrip, p_color);
        hue += cidx << 4;
        _hue(hue, &led);
    }
}
// }}}

// {{{ void pattern_sourcery(void)
void pattern_sourcery(void) {
    numc = 2;
    uint8_t hues[2] = {2, 34};

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            if (velocity < 40) {
                p_color = interp_v(0, 6, 10, 30);
                p_blank = interp_v(100, 34, 10, 30);
            } else {
                p_color = interp_v(6, 3, 45, 75);
                p_blank = interp_v(34, 17, 45, 75);
            }
        }

        if (segm == 0) {
            segm = 1;
            trip = p_blank;
        } else {
            segm = 0;
            trip = p_color;
            cidx++;
            if (cidx >= numc) cidx = 0;
        }
    }


    if (segm == 1) {
        HSV hsv;
        uint8_t vel = (velocity < 30) ? 0 : (velocity > 60) ? 30 : velocity - 30;
        uint8_t s = 16 - (norm64(vel) >> 2);

        _hue(hues[cidx], &led);
        _sat(s, &led);
    }
}
// }}}

// {{{ void pattern_bleeding_edge(void)
void pattern_bleeding_edge(void) {
    if (timers[0] >= 250) {
        counters[0]++;
        if (counters[0] >= (192 << 1)) counters[0] = 0;
    }

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            p_blank = interp_v(75, 25, 10, 60);
            p_color = interp_v(8, 2, 10, 60);
            counters[1] = (velocity > 80) ? 96 : velocity + 16;
        }

        if (segm == 0) {
            segm = 1;
            trip = p_blank;
        } else if (segm == 1) {
            segm = 2;
            trip = p_color;
        } else if (segm == 2) {
            segm = 3;
            trip = p_color;
        } else if (segm == 3) {
            segm = 0;
            trip = p_color;
            cidx++;
            if (cidx >= 2) cidx = 0;
        }
    }

    uint8_t itrip = p_color - trip;
    uint16_t h = (counters[0] < 192) ? counters[0] : (192 << 1) - counters[0];
    uint8_t o = 0;
    uint8_t vx = 0;

    if (segm != 0) {
        if (segm == 1) {
            // blend from [delta -> 0)
            vx = norm64(itrip, p_color) >> 2;
            o = interp(counters[1], 0, itrip, p_color);
        } else if (segm == 3) {
            // blend from (0 -> delta]
            vx = norm64(itrip, p_color) >> 2;
            itrip += 1;
            o = interp(0, counters[1], 0, itrip, p_color);
        }

        h += (cidx == 0) ? o : 192 - o;
        if (h >= 192) h -= 192;
        _hue(h, &led);
        _val(vx, &led);
    }
}
// }}}

// {{{ void pattern_ghost(void)
void pattern_ghost(void) {
    uint8_t split = 24;
    if (timers[0] >= 250) {
        counters[0]++;
        if (counters[0] >= 192) counters[0] = 0;
    }

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            p_color = interp_v(34, 8, 20, 75);
            p_blank = 20;
            p_tcolor = 8;
            timers[1] = interp_v(1, 4, 20, 75);
        }

        if (segm == 0) {
            segm = 1;
            trip = p_blank;
        } else {
            segm = 0;
            counters[1]++;
            if (counters[1] > timers[1]) {
                counters[1] = 0;
                cidx++;
                if (cidx > 2) cidx = 0;
            }
            trip = (counters[1] == timers[1]) ? p_color : p_tcolor;
        }
    }

    uint16_t hue = counters[0];
    uint8_t val = 0;

    if (segm == 1) {
        if (counters[1] == timers[1]) {
            if (cidx == 0) {
                hue += 192 - split;
            } else if (cidx == 2) {
                hue += split;
            }
        } else {
            val = 12;
            hue += 96;
        }

        if (hue >= 192) hue -= 192;
        _hue(hue, &led);
        _val(val, *led);
    }
}
// }}}

// {{{ void pattern_wave_runner(void)
void pattern_wave_runner(void) {
    const uint8_t parts = 32;
    const uint8_t halfp = parts >> 1;

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            p_blank = interp_v(16, 0, 10, 50);
            p_blank_end = interp_v(16, 32, 10, 50);
            p_color = interp_v(10, 3, 10, 50);
        }

        if (segm == 0) {
            segm = 1;
            if (counters[0] < halfp) {
                trip = interp(p_blank, p_blank_end, counters[0], halfp);
            } else {
                trip = interp(p_blank_end, p_blank, counters[0] - halfp, halfp);
            }
        } else {
            segm = 0;
            trip = p_color;
            counters[0]++;
            if (counters[0] >= parts) counters[0] = 0;
            counters[1]++;
            if (counters[1] >= 4) counters[1] = 0;
            cidx++;
            if (cidx > 2) cidx = 0;
        }
    }

    uint16_t hue = 0;
    uint8_t sat = counters[1] << 2;
    if (segm == 1) {
        hue += cidx << 4;
        if (velocity < 50) {
            hue += interp_v(0, 64, 0, 50);
        } else {
            hue += interp_v(64, 128, 50, 80);
        }
        _hue(hue, &led);
        _sat(sat, &led);
    }
}
// }}}

// {{{ void pattern_rainbows(void)
void pattern_rainbows(void) {
    if (timers[0] >= 150) {
        counters[0]++;
        if (counters[0] >= 192) counters[0] = 0;
    }

    if (timers[1] >= 250) {
        counters[1]++;
        if (counters[1] >= 192) counters[0] = 0;
    }

    if (timers[2] >= 350) {
        counters[2]++;
        if (counters[2] >= 192) counters[0] = 0;
    }

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            numc = (velocity < 20) ? 1 : (velocity < 50) ? 2 : 3;  
            p_color = interp_v(20, 5, 20, 80);
            p_blank = interp_v(30, 15, 20, 80);
        }

        if (segm == 0) {
            segm = 1;
            trip = p_blank;
        } else {
            segm = 0;
            trip = p_color;
            cidx++;
            if (cidx >= numc) cidx = 0;
        }
    }

    if (segm == 1) {
        _hue(counters[cidx], &led);
    }
}
// }}}

// {{{ void pattern_arrowhead(void)
void pattern_arrowhead(void) {
    const uint8_t parts = 4;
    const uint8_t tparts = parts + parts + 1;

    if (timers[0] >= 250) {
        counters[0]++;
        if (counters[0] >= 192) counters[0] = 0;
    }

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            p_color = interp_v(10, 4, 20, 70);
            p_color_end = interp_v(4, 1, 20, 70);
            p_blank = interp_v(8, 2, 20, 70);
            p_gap = interp_v(75, 35, 20, 70);
        }

        if (segm == 0) {
            segm = 1;
            if (counters[1] == 0) {
                trip = p_gap;
            } else {
                trip = p_blank;
            }
        } else {
            segm = 0;
            counters[1]++;
            if (counters[1] >= tparts) counters[1] = 0;

            if (counters[1] < parts) {
                trip = interp(p_color_end, p_color, counters[1], parts - 1);
            } else if (counters[1] == parts) {
                trip = p_color;
            } else {
                trip = interp(p_color, p_color_end, counters[1] - parts, parts - 1);
            }
        }
    }

    uint8_t satval = 0;
    if (segm == 1) {
        if (counters[1] < parts) {
            satval = ((4 - counters[1]) << 2) - 1;
        } else if (counters[1] > parts) {
            satval = ((counters[1] - 4) << 2) - 1;
        }

        _hue(counters[0], &led);
        _sat(satval, &led);
        _val(satval, &led);
    }
}
// }}}

// {{{ void pattern_breaker(void)
void pattern_breaker(void) {
    if (timers[0] >= 250) {
        counters[0]++;
        if (counters[0] >= 192) counters[0] = 0;
    }

    while (trip == 0) {
        if (new_reading) {
            new_reading = false;
            p_gap = interp_v(5, 20, 20, 80);
            p_color = interp_v(5, 3, 20, 80);
            p_blank = interp_v(0, 10, 20, 80);
            p_tcolor = interp_v(8, 0, 20, 60);
            if (velocity < 60) {
                p_tblank = interp_v(0, 8, 20, 60);
            } else {
                p_tblank = interp_v(8, 0, 60, 80);
            }
        }

        if (segm == 0) {
            segm = 1;
            if (counters[2] == 0) {
                trip = p_gap;
            } else if (counters[2] < 5) {
                trip = p_tblank;
            } else if (counters[2] == 5) {
                trip = p_gap;
            } else {
                trip = p_blank;
            }
        } else {
            segm = 0;
            
            if (counters[1] == 0) {
                counters[2]++;
                if (counters[2] >= 5) {
                    counters[2] = 0;
                    counters[1] = 1;
                    cidx++;
                    if (cidx >= 2) cidx = 0;
                }
            } else {
                counters[2]++;
                if (counters[2] >= 3) {
                    counters[2] = 0;
                    counters[1] = 0;
                }
            }

            trip = (counters[1] == 0) ? p_tcolor : p_color;
        }
    }

    RGB target;
    uint8_t nx;
    if (segm == 1) {
        if (counters[1] == 0) {
            if (cidx == 0) {
                _hue(counters[0], &led);
            } else {
                _hue(192 - counters[0], &led);
            }
        } else {
            _hue(116, &led);
            _sat(14, &led);

            if (velocity > 60) {
                nx = ((velocity > 76) ? 16 : velocity - 60) << 2;
                if (counters[2] == 0) {
                    _hue(counters[0], &target);
                } else if (counters[2] == 2) {
                    _hue(192 - counters[0], &led);
                }
                led.r = interp64(led.r, target.r, nx);
                led.g = interp64(led.g, target.g, nx);
                led.b = interp64(led.b, target.b, nx);
            }
        }
    }
}
// }}}
// }}}

// {{{ ACCEL
void accel_send(uint8_t addr, uint8_t data) {
    I2C1_MESSAGE_STATUS status;
    uint8_t write_buffer[2];
    uint8_t timeOut = 0;
    write_buffer[0] = addr;
    write_buffer[1] = data;
    while (status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterWrite(write_buffer, 2, ACCEL_ADDR, &status);
        while (status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE) break;
        if (timeOut == 100) break;
        else timeOut++;
    }
}

void accel_read(void) {
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK readTRB[2];
    uint8_t write_buffer[1];
    uint16_t timeOut = 0;

    status = I2C1_MESSAGE_PENDING;
    write_buffer[0] = 0x00;
    I2C1_MasterWriteTRBBuild(&readTRB[0], write_buffer, 1, ACCEL_ADDR);
    I2C1_MasterReadTRBBuild(&readTRB[1], accel_buffer, 7, ACCEL_ADDR);

    while (status != I2C1_MESSAGE_FAIL) {
        I2C1_MasterTRBInsert(2, readTRB, &status);
        while (status == I2C1_MESSAGE_PENDING);
        if (status == I2C1_MESSAGE_COMPLETE) break;
        if (timeOut == 100) break;
        else timeOut++;
    }
}

void accel_convert(void) {
    AXIS_X = (int16_t)accel_buffer[1] << 8;
    AXIS_X = (AXIS_X | accel_buffer[2]) >> 4;
    AXIS_Y = (int16_t)accel_buffer[3] << 8;
    AXIS_Y = (AXIS_Y | accel_buffer[4]) >> 4;
    AXIS_Z = (int16_t)accel_buffer[5] << 8;
    AXIS_Z = (AXIS_Z | accel_buffer[6]) >> 4;
    AXIS_X2 = (AXIS_X >= 0) ? AXIS_X : -AXIS_X;
    AXIS_Y2 = (AXIS_Y >= 0) ? AXIS_Y : -AXIS_Y;
    AXIS_Z2 = (AXIS_Z >= 0) ? AXIS_Z : -AXIS_Z;
}

void accel_velocity_start(void) {
    magnitude  = 0;
    magnitude += AXIS_X2;
    magnitude += AXIS_Y2;
    magnitude += AXIS_Z2;
    new_velocity = 0;
    accel_thresh = ACCEL_THRESH_START;
    accel_thresh_d = accel_thresh_dv;
    accel_bin = 0;
}

void accel_velocity(uint8_t stop) {
    while (accel_bin < stop) {
        accel_thresh += accel_thresh_d;
        accel_thresh_d += accel_thresh_dd;

        if (magnitude > accel_thresh) {
            falloff[accel_bin] = 0;
            if (trigger[accel_bin] < 128) {
                trigger[accel_bin]++;
            }
        }

        if (falloff[accel_bin] > falloff_value)
            trigger[accel_bin] = 0;

        if (trigger[accel_bin] > trigger_value)
            new_velocity = accel_bin + 1;

        falloff[accel_bin]++;
        accel_bin++;
    }
}

void accel_velocity_end(void) {
    velocity = new_velocity;
    new_reading = true;
}

void accel_standby(void) {
    accel_send(MMA_CTRL_REG1, 0x00);
}

void accel_init(void) {
    accel_send(MMA_CTRL_REG1, 0x00);
    accel_send(MMA_PL_CFG_REG, MMA_PL_CFG_REG_);//Jarrad Delete this if needed
    accel_send(MMA_XYZ_DATA_CFG, MMA_XYZ_DATA_CFG_);
    accel_send(MMA_CTRL_REG2, MMA_CTRL_REG2_);
    accel_send(MMA_CTRL_REG3, MMA_CTRL_REG3_);
    accel_send(MMA_CTRL_REG4, MMA_CTRL_REG4_);
    accel_send(MMA_CTRL_REG5, MMA_CTRL_REG5_);
    accel_send(MMA_CTRL_REG1, MMA_CTRL_REG1_);

}
// }}}

// {{{ HARDWARE SHIT
void enter_sleep(void) {
  accel_standby();

  LED_R = 0;
  LED_G = 0;
  LED_B = 0;

  PIE0bits.IOCIE = 1;
  CCP1CON = 0x00;
  CCP2CON = 0x00;
  CCP3CON = 0x00;
  T2CONbits.TMR2ON = 0;
  T4CONbits.TMR4ON = 0;
  IOCAF = 0;
  IOCCF = 0;

  SLEEP();
  NOP();
  NOP();

  IOCAF = 0;
  IOCCF = 0;
  T2CONbits.TMR2ON = 1;
  T4CONbits.TMR4ON = 1;
  CCP1CON = 0x8F;
  CCP2CON = 0x8F;
  CCP3CON = 0x8F;
  PIE0bits.IOCIE = 0;

  LED_R = 0;
  LED_G = 0;
  LED_B = 0;

  accel_init();
  since_press = 0;
  cstate = STATE_OFF;
  mode_idx = 0;
  init_mode();
}

void interrupt INTERRUPT_InterruptManager (void) {
  if (INTCONbits.PEIE == 1 && PIE1bits.BCL1IE == 1 && PIR1bits.BCL1IF == 1) {
    PIR1bits.BCL1IF = 0;
  } else if (INTCONbits.PEIE == 1 && PIE1bits.SSP1IE == 1 && PIR1bits.SSP1IF == 1) {
    PIR1bits.SSP1IF = 0;
    I2C1_ISR();
  } else if (INTCONbits.PEIE == 1 && PIE2bits.TMR4IE == 1 && PIR2bits.TMR4IF == 1) {
    PIR2bits.TMR4IF = 0;
    TMR4 = 0;
    frame_locked = false;
  } else if (PIE0bits.IOCIE == 1 && PIR0bits.IOCIF == 1 && IOCCFbits.IOCCF2 == 1) {
    IOCCFbits.IOCCF2 = 0;
  } else if (PIR0) {
    PIR0 = 0;
  } else {
    // unhandled
  }
}
// }}}

// {{{ void handle_render(void)
void init_mode(void) {
    for (uint8_t i = 0; i < 8; i++) {
        timers[i] = 0;
        counters[i] = 0;
    }
    trip = 0;
    segm = 0;
    cidx = 0;

    p_color = 0;
    p_color_end = 0;
    p_blank = 0;
    p_blank_end = 0;
    p_gap = 0;
    p_tcolor = 0;
    p_tblank = 0;

    velocity = new_velocity = 0;
    new_reading = true;
}

void handle_render(void) {
    if (cstate == STATE_ON) {
        led.r = led.g = led.b = 0;

        if (!was_pressed) {
            switch (mode_idx) {
                case 0: pattern_darkside(); break;
                case 1: pattern_sourcery(); break;
                case 2: pattern_bleeding_edge(); break;
                case 3: pattern_ghost(); break;
                case 4: pattern_wave_runner(); break;
                case 5: pattern_rainbows(); break;
                case 6: pattern_arrowhead(); break;
                case 7: pattern_breaker(); break;
                default: mode_idx = 0;
            }
        }

        trip--;
        timers[0]++;
        timers[1]++;
        timers[2]++;
        timers[3]++;
        timers[4]++;
        timers[5]++;
        timers[6]++;
        timers[7]++;
    }
}
// }}}

// {{{ void handle_button(void)
void handle_button(void) {
    bool pressed = BUTTON_PRESSED;
    bool changed = pressed != was_pressed;

    if (pressed) {
    } else if (changed) {
        if (cstate == STATE_OFF) {
            if (since_press < 2000) {
                enter_sleep();
            } else {
                cstate = STATE_ON;
            }
        } else if (cstate == STATE_ON) {
            if (since_press < 700) {
                mode_idx++;
                if (mode_idx >= NUM_MODES) mode_idx = 0;
            } else {
                enter_sleep();
            }
        }
    }

    since_press++;
    if (changed) since_press = 0;
    was_pressed = pressed;
}
// }}}

// {{{ void handle_accel(void)
void handle_accel(void) {
    switch (accel_step) {
        case  0: accel_read();                break;
        case  1: accel_convert();             break;
        case  2: AXIS_X2 = AXIS_X2 * AXIS_X2; break;
        case  3: AXIS_Y2 = AXIS_Y2 * AXIS_Y2; break;
        case  4: AXIS_Z2 = AXIS_Z2 * AXIS_Z2; break;
        case  5: accel_velocity_start();      break;
        case  6: accel_velocity(25);          break;
        case  7: accel_velocity(50);          break;
        case  8: accel_velocity(75);          break;
        case  9: accel_velocity(100);         break;
        case 10: accel_velocity_end();        break;
        default: break;
    }

    accel_step++;
    if (accel_step >= 40) accel_step = 0;
}
// }}}

// {{{ void init(void)
void init(void) {
    LATA = 0x37;
    TRISA = 0x20;
    ANSELA = 0x00;
    WPUA = 0x00;
    ODCONA = 0x00;

    TRISC = 0x07;
    LATC = 0x00;
    ANSELC = 0x38;
    WPUC = 0x00;
    ODCONC = 0x00;

    IOCCFbits.IOCCF2 = 0;
    IOCCNbits.IOCCN2 = 1;
    IOCCPbits.IOCCP2 = 1;

    bool state = GIE;
    GIE = 0;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00;       // unlock PPS

    RC0PPSbits.RC0PPS = 0x18;           //RC0->MSSP1:SCL1;
    RC1PPSbits.RC1PPS = 0x19;           //RC1->MSSP1:SDA1;
    SSP1CLKPPSbits.SSP1CLKPPS = 0x10;   //RC0->MSSP1:SCL1;
    SSP1DATPPSbits.SSP1DATPPS = 0x11;   //RC1->MSSP1:SDA1;

    RC5PPSbits.RC5PPS = 0x0C;           //RC5->CCP1:CCP1;
    RC4PPSbits.RC4PPS = 0x0D;           //RC4->CCP2:CCP2;
    RC3PPSbits.RC3PPS = 0x0E;           //RC3->CCP3:CCP3;

    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01;       // lock PPS

    GIE = state;

    OSCCON1 = 0x60;
    OSCCON3 = 0x00;
    OSCEN = 0x00;
    OSCFRQ = 0x06;
    OSCTUNE = 0x00;

    WDTCON = 0x16;

    T2CON = 0x00;
    PR2 = 0x3F;
    TMR2 = 0x00;
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;

    T4CON = 0x02;
    PR4 = 0x7C;
    TMR4 = 0x00;
    PIR2bits.TMR4IF = 0;
    PIE2bits.TMR4IE = 1;
    T4CONbits.TMR4ON = 1;

    CCP1CON = 0x8F;
    CCPR1H = 0x00;
    CCPR1L = 0x00;
    CCPTMRSbits.C1TSEL = 0x1;

    CCP2CON = 0x8F;
    CCPR2H = 0x00;
    CCPR2L = 0x00;
    CCPTMRSbits.C2TSEL = 0x1;

    CCP3CON = 0x8F;
    CCPR3H = 0x00;
    CCPR3L = 0x00;
    CCPTMRSbits.C3TSEL = 0x1;

    I2C1_Initialize();
}
// }}}

// {{{ void main(void)
void main(void) {
    init();
    GIE = 1;
    PEIE = 1;
    accel_init();
    accel_thresh_dv = ACCEL_THRESH_DS[accel_thresh_range];
    accel_thresh_dd = ACCEL_THRESH_DDS[accel_thresh_range];
    enter_sleep();

    while (1) {
        frame_locked = true;

        handle_button();
        handle_accel();
        handle_render();

        while (frame_locked);
        LED_R = led.r;
        LED_G = led.g;
        LED_B = led.b;
        CLRWDT();
    }
}
// }}}
