#include "quark.h"

// {{{ globals
Mode          mode;
HSV           edit_color;
RGB           frame;

uint8_t       accel_profile = 2;
uint8_t       accel_bin;
uint8_t       accel_velocity;
uint8_t       accel_tick;
uint16_t      accel_thresh_d;
uint16_t      accel_thresh_dd;
uint32_t      accel_thresh;
int16_t       axis;
uint32_t      mag;
uint8_t       falloff[64];
uint8_t       trigger[64];

uint32_t      last_write;
uint32_t      since_press;
uint16_t      flash_tick;
uint16_t      flash_ticks;
uint8_t       mode_idx;
uint8_t       op_state;
uint8_t       flash_color;
uint8_t       was_pressed;                // 20
// color.h                                // 64
                                          // 84 + 1175 = 1259
// }}}

uint8_t interp_value(uint8_t values[4]) {
  return interp_f(values[mode.iv_s], values[mode.iv_e], mode.iv_f);
}

void flash(uint8_t c) {
  flash_color = c;
  flash_ticks = 1000;
  flash_tick = 0;
}

// {{{ accel
inline void I2CADC_SDA_H_OUTPUT(void) { DDRC &= ~(1 << 4); }
inline void I2CADC_SDA_L_INPUT(void)  { DDRC |=  (1 << 4); }
inline void I2CADC_SCL_H_OUTPUT(void) { DDRC &= ~(1 << 5); }
inline void I2CADC_SCL_L_INPUT(void)  { DDRC |=  (1 << 5); }

void TWADC_write(uint8_t data) {
  uint8_t data_r = ~data;
  uint8_t i = 8;
  while (i--) {
    pinMode(SDA_PIN, bitRead(data_r, i));
    I2CADC_SCL_H_OUTPUT();
    I2CADC_SCL_L_INPUT();
  }

AckThis:
  I2CADC_SCL_L_INPUT(); 
  I2CADC_SCL_H_OUTPUT();
  int ADCresult = analogRead(SCL_PIN);
  if (ADCresult < I2CADC_L) {
    goto AckThis;
  }
  I2CADC_SCL_L_INPUT();
}

uint8_t TWADC_read(bool ack) {
  uint8_t data = 0;
  uint8_t i = 8;
  while (i--) {
    I2CADC_SDA_H_OUTPUT();
    I2CADC_SCL_H_OUTPUT();
    int result = analogRead(SDA_PIN);
    if (result < I2CADC_L) {
      data &= ~(1 << i);
    } else {
      data |= (1 << i);
    }
    I2CADC_SCL_L_INPUT();
  }

  if (ack) {
    I2CADC_SCL_L_INPUT();  NOP;
    I2CADC_SDA_L_INPUT();
    I2CADC_SCL_H_OUTPUT(); NOP;
    I2CADC_SCL_L_INPUT();  NOP;
  } else {
AckThis:
    I2CADC_SCL_L_INPUT();  NOP;
    I2CADC_SCL_H_OUTPUT(); NOP;
    int result = analogRead(SCL_PIN);
    if (result < I2CADC_L) {
      goto AckThis;
    }
    I2CADC_SCL_L_INPUT();  NOP;

  }
  return data;
}

void TWADC_write_w(uint8_t data) {
  uint8_t data_r = ~data;
  uint8_t i = 7;
  while (i--) {
    pinMode(SDA_PIN, bitRead(data_r, i));
    I2CADC_SCL_H_OUTPUT();
    I2CADC_SCL_L_INPUT();
  }
  I2CADC_SDA_L_INPUT();
  I2CADC_SCL_H_OUTPUT();
  I2CADC_SCL_L_INPUT();
AckThis:
  I2CADC_SCL_L_INPUT();
  I2CADC_SCL_H_OUTPUT();
  int result = analogRead(SCL_PIN);
  if (result < I2CADC_L) {
    goto AckThis;
  }
  I2CADC_SCL_L_INPUT();
}

void TWADC_write_r(uint8_t data) {
  uint8_t data_r = ~data;
  uint8_t i = 7;
  while (i--) {
    i--;
    pinMode(SDA_PIN, bitRead(data_r, i));
    I2CADC_SCL_H_OUTPUT();
    I2CADC_SCL_L_INPUT();
  }
  I2CADC_SDA_H_OUTPUT();
  I2CADC_SCL_H_OUTPUT();
  I2CADC_SCL_L_INPUT();
AckThis:
  I2CADC_SCL_L_INPUT();
  I2CADC_SCL_H_OUTPUT();
  int result = analogRead(SCL_PIN);
  if (result < I2CADC_L) {
    goto AckThis;
  }
  I2CADC_SCL_L_INPUT();
}

void TWADC_begin(void) {
  I2CADC_SCL_H_OUTPUT();

  I2CADC_SDA_H_OUTPUT();
  I2CADC_SDA_L_INPUT();

  I2CADC_SCL_H_OUTPUT();
  I2CADC_SCL_L_INPUT();
}

void TWADC_beginTransmission(uint8_t addr) {
  TWADC_begin();
  TWADC_write_w(addr);
}

void TWADC_endTransmission(void) {
  I2CADC_SDA_L_INPUT();
  I2CADC_SCL_H_OUTPUT();
  I2CADC_SDA_H_OUTPUT();
}

void TWADC_send(uint8_t addr, uint8_t data) {
  TWADC_beginTransmission(ACCEL_ADDR);
  TWADC_write(addr);
  TWADC_write(data);
  TWADC_endTransmission();
  delay(1);
}

void accel_init(void) {
  TWADC_begin();
  delay(1);
  TWADC_send(0x2A, B00000000); // Standby to accept new settings
  TWADC_send(0x0E, B00000001); // Set +-4g range
  TWADC_send(0x2B, B00011011); // Low Power
  TWADC_send(0x2C, B00000000); // No interrupt wake
  TWADC_send(0x2D, B00000000); // No interrupts
  TWADC_send(0x2E, B00000000); // Interrupts on INT2
  TWADC_send(0x2A, B00100001); // Set 50Hz and active
}

void accel_standby(void) {
  TWADC_send(0x2A, 0x00);
}
// }}}

// {{{ interp
const uint16_t RECIPROCALS[256] PROGMEM = {
  0x0000, 0xFFFF, 0x8000, 0x5555, 0x4000, 0x3333, 0x2AAA, 0x2492,
  0x2000, 0x1C71, 0x1999, 0x1745, 0x1555, 0x13B1, 0x1249, 0x1111,
  0x1000, 0x0F0F, 0x0E38, 0x0D79, 0x0CCC, 0x0C30, 0x0BA2, 0x0B21,
  0x0AAA, 0x0A3D, 0x09D8, 0x097B, 0x0924, 0x08D3, 0x0888, 0x0842,
  0x0800, 0x07C1, 0x0787, 0x0750, 0x071C, 0x06EB, 0x06BC, 0x0690,
  0x0666, 0x063E, 0x0618, 0x05F4, 0x05D1, 0x05B0, 0x0590, 0x0572,
  0x0555, 0x0539, 0x051E, 0x0505, 0x04EC, 0x04D4, 0x04BD, 0x04A7,
  0x0492, 0x047D, 0x0469, 0x0456, 0x0444, 0x0432, 0x0421, 0x0410,
  0x0400, 0x03F0, 0x03E0, 0x03D2, 0x03C3, 0x03B5, 0x03A8, 0x039B,
  0x038E, 0x0381, 0x0375, 0x0369, 0x035E, 0x0353, 0x0348, 0x033D,
  0x0333, 0x0329, 0x031F, 0x0315, 0x030C, 0x0303, 0x02FA, 0x02F1,
  0x02E8, 0x02E0, 0x02D8, 0x02D0, 0x02C8, 0x02C0, 0x02B9, 0x02B1,
  0x02AA, 0x02A3, 0x029C, 0x0295, 0x028F, 0x0288, 0x0282, 0x027C,
  0x0276, 0x0270, 0x026A, 0x0264, 0x025E, 0x0259, 0x0253, 0x024E,
  0x0249, 0x0243, 0x023E, 0x0239, 0x0234, 0x0230, 0x022B, 0x0226,
  0x0222, 0x021D, 0x0219, 0x0214, 0x0210, 0x020C, 0x0208, 0x0204,
  0x0200, 0x01FC, 0x01F8, 0x01F4, 0x01F0, 0x01EC, 0x01E9, 0x01E5,
  0x01E1, 0x01DE, 0x01DA, 0x01D7, 0x01D4, 0x01D0, 0x01CD, 0x01CA,
  0x01C7, 0x01C3, 0x01C0, 0x01BD, 0x01BA, 0x01B7, 0x01B4, 0x01B2,
  0x01AF, 0x01AC, 0x01A9, 0x01A6, 0x01A4, 0x01A1, 0x019E, 0x019C,
  0x0199, 0x0197, 0x0194, 0x0192, 0x018F, 0x018D, 0x018A, 0x0188,
  0x0186, 0x0183, 0x0181, 0x017F, 0x017D, 0x017A, 0x0178, 0x0176,
  0x0174, 0x0172, 0x0170, 0x016E, 0x016C, 0x016A, 0x0168, 0x0166,
  0x0164, 0x0162, 0x0160, 0x015E, 0x015C, 0x015A, 0x0158, 0x0157,
  0x0155, 0x0153, 0x0151, 0x0150, 0x014E, 0x014C, 0x014A, 0x0149,
  0x0147, 0x0146, 0x0144, 0x0142, 0x0141, 0x013F, 0x013E, 0x013C,
  0x013B, 0x0139, 0x0138, 0x0136, 0x0135, 0x0133, 0x0132, 0x0130,
  0x012F, 0x012E, 0x012C, 0x012B, 0x0129, 0x0128, 0x0127, 0x0125,
  0x0124, 0x0123, 0x0121, 0x0120, 0x011F, 0x011E, 0x011C, 0x011B,
  0x011A, 0x0119, 0x0118, 0x0116, 0x0115, 0x0114, 0x0113, 0x0112,
  0x0111, 0x010F, 0x010E, 0x010D, 0x010C, 0x010B, 0x010A, 0x0109,
  0x0108, 0x0107, 0x0106, 0x0105, 0x0104, 0x0103, 0x0102, 0x0101,
};

uint32_t get_f(uint16_t x, uint16_t d) {
  if (x == 0) return 0;
  if (x == d) return 0xFFFFFFFF;

  while (d >= 256) {
    d >>= 1;
    x >>= 1;
  }
  return x * pgm_read_byte(&RECIPROCALS[d]);
}

uint8_t interp_f(uint8_t s, uint8_t e, uint32_t f) {
  union tmp {
    uint32_t v;
    uint8_t  b[4];
  } tmp;

  if (s == e)          return s;
  if (f == 0)          return s;
  if (f == 0xFFFFFFFF) return e;

  tmp.v = f;
  if (s < e) {
    tmp.v *= e - s;
    return s + tmp.b[2];
  } else {
    tmp.v *= s - e;
    return s - tmp.b[2];
  }
}

uint8_t interp(uint8_t s, uint8_t e, uint16_t x, uint16_t d) {
  return interp_f(s, e, get_f(x, d));
}
// }}}

// {{{ color
void hue_color(uint8_t hue, RGB *rgb) {
  if (hue >= 24) hue -= 24;
  if (hue < 8) {
    rgb->r = VALS_R[8 - hue];
    rgb->g = VALS_G[hue];
    rgb->b = 0;
  } else if (hue < 16) {
    rgb->r = 0;
    rgb->g = VALS_G[16 - hue];
    rgb->b = VALS_B[hue - 8];
  } else {
    rgb->r = VALS_R[hue - 16];
    rgb->g = 0;
    rgb->b = VALS_B[24 - hue];
  }
}

uint8_t sat_channel(uint8_t sat, uint8_t color, uint8_t white) {
  uint16_t frac = (color < white) ? FRAC8U[sat] : FRAC8D[sat];
  return interp_f(color, white, frac);
}

void sat_color(uint8_t sat, RGB *rgb) {
  rgb->r = sat_channel(sat, rgb->r, VALS_R[3]);
  rgb->g = sat_channel(sat, rgb->g, VALS_G[3]);
  rgb->b = sat_channel(sat, rgb->b, VALS_B[3]);
}

void val_color(uint8_t val, RGB *rgb) {
  uint16_t frac = FRAC8D[val];
  rgb->r = interp_f(rgb->r, 0, frac);
  rgb->g = interp_f(rgb->g, 0, frac);
  rgb->b = interp_f(rgb->b, 0, frac);
}

void unhsv(HSV *hsv, uint16_t rot, RGB *rgb) {
  if (hsv->v >= 8) {
    rgb->r = 0;
    rgb->g = 0;
    rgb->b = 0;
    return;
  }

  if (hsv->s >= 8) {
    rgb->r = VALS_R[3];
    rgb->g = VALS_G[3];
    rgb->b = VALS_B[3];
    return;
  }

  uint16_t r = hsv->h + rot;
  while (r >= 192) r -= 192;
  uint8_t x = r >> 3;
  uint8_t y = r & 0x7;

  hue_color(x, rgb);

  if (y > 0) {
    RGB e;
    hue_color(x + 1, &e);

    uint32_t frac = FRAC8C[y];
    rgb->r = interp_f(rgb->r, e.r, frac);
    rgb->g = interp_f(rgb->g, e.g, frac);
    rgb->b = interp_f(rgb->b, e.b, frac);
  }

  if (hsv->s > 0) sat_color(hsv->s, rgb);
  if (hsv->v > 0) val_color(hsv->v, rgb);
}

void blend_f(HSV *c0, HSV *c1, uint32_t frac, uint16_t rot, RGB *rgb) {
  RGB r0, r1;
  unhsv(c0, rot, &r0);
  unhsv(c1, rot, &r1);

  rgb->r = interp_f(r0.r, r1.r, frac);
  rgb->g = interp_f(r0.g, r1.g, frac);
  rgb->b = interp_f(r0.b, r1.b, frac);
}

void blend(HSV *c0, HSV *c1, uint16_t x, uint16_t d, uint16_t rot, RGB *rgb) {
  uint32_t frac = get_f(x, d);
  blend_f(c0, c1, frac, rot, rgb);
}
// }}}

// {{{ power
void _push_interrupt() {
  sleep_disable();
  detachInterrupt(0);
}

void power_down(void) {
  // Set up sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(0, _push_interrupt, FALLING);
  ADCSRA = 0;

  // Go to sleep here
  cli();
  sleep_bod_disable();
  sei();
  sleep_cpu();

  // Wake up here
  sleep_disable();
  detachInterrupt(0);
}

void enter_sleep(void) {
  digitalWrite(PIN_LDO, LOW);
  analogWrite(PIN_R, 0);
  analogWrite(PIN_G, 0);
  analogWrite(PIN_B, 0);
  while (true) {}
}
// }}}

// {{{ load mode and init state
void load_mode(void) {
  uint16_t i = MODE_SIZE;
  while (--i) mode.data[i] = pgm_read_byte(&FACTORY[mode_idx][i]);
  init_state();
}

void init_state(void) {
  // init global values
  accel_tick       = 0;
  mode.velocity    = 0;
  mode.falloff     = mode.falloff_v[0];
  mode.trigger     = mode.trigger_v[0];
  mode.iv_s        = 0;
  mode.iv_e        = 0;
  mode.iv_f        = 0;
  mode.segment_num = 0;
  mode.strobe_num  = 0;
  mode.part        = 0;
  mode.trip        = 0;
  mode.seg_len     = mode.segments[0].strobe_v[0];

  for (uint8_t i = 0; i < NUM_SEGMENTS; i++) {
    mode.segments[i].cidx         = 0;
    mode.segments[i].repeat_num   = 0;
    mode.segments[i].change_num   = 0;
    mode.segments[i].flip_rot     = 0;
    mode.segments[i].flip_effect  = 0;
    mode.segments[i].strobe       = mode.segments[i].strobe_v[0];
    mode.segments[i].strobe_end   = mode.segments[i].strobe_end_v[0];
    mode.segments[i].blank        = mode.segments[i].blank_v[0];
    mode.segments[i].blank_end    = mode.segments[i].blank_end_v[0];
    mode.segments[i].gap          = mode.segments[i].gap_v[0];
    mode.segments[i].strobe_count = mode.segments[i].strobe_count_v[0];
    mode.segments[i].repeat_count = mode.segments[i].repeat_count_v[0];
    mode.segments[i].change_count = mode.segments[i].change_count_v[0];

    // init color state all colors
    for (uint8_t j = 0; j < 8; j++) {
      mode.segments[i].palette[j].timer   = mode.segments[i].palette[j].timer_v[0];
      mode.segments[i].palette[j].tick    = 0;
      mode.segments[i].palette[j].repeat  = 0;
      mode.segments[i].palette[j].counter = 0;
      mode.segments[i].palette[j].cidx    = 0;
    }
  }
}
// }}}

// {{{ handle accel
void accel_interp(void) {
  mode.iv_s = 0;
  mode.iv_e = 0;
  mode.iv_f = 0;

  if (mode.velocity < mode.thresh[0]) {
    // pass
  } else if (mode.numt > 1 && mode.velocity < mode.thresh[1]) {
    if (mode.jump[0] != 0) {
      mode.iv_e++;
      mode.iv_f = get_f(mode.velocity - mode.thresh[0], mode.thresh[1] - mode.thresh[0]);
    }
  } else if (mode.numt > 2 && mode.velocity < mode.thresh[2]) {
    mode.iv_s = 1;
    mode.iv_e = 1;
    if (mode.jump[1] != 0) {
      mode.iv_e++;
      mode.iv_f = get_f(mode.velocity - mode.thresh[1], mode.thresh[2] - mode.thresh[1]);
    }
  } else if (mode.numt > 3 && mode.velocity < mode.thresh[3]) {
    mode.iv_s = 2;
    mode.iv_e = 2;
    if (mode.jump[2] != 0) {
      mode.iv_e++;
      mode.iv_f = get_f(mode.velocity - mode.thresh[2], mode.thresh[3] - mode.thresh[2]);
    }
  } else {
    mode.iv_s = mode.numt;
    mode.iv_e = mode.numt;
  }
}

void accel_check_bins(uint8_t last) {
  while (accel_bin < last) {
    accel_thresh  += accel_thresh_d;
    if (mag > accel_thresh) { if (trigger[accel_bin] < 128) trigger[accel_bin]++; }
    else                    { if (falloff[accel_bin] < 128) falloff[accel_bin]++; }
    if (falloff[accel_bin] > mode.falloff) {
      falloff[accel_bin] = 0;
      trigger[accel_bin] = 0;
    }
    if (trigger[accel_bin] > mode.trigger) accel_velocity++;
    accel_bin++;
    accel_thresh_d += accel_thresh_dd;
  }
}

void accel_calc_timers(uint8_t s) {
  for (uint8_t i = 0; i < mode.segments[s].numc; i++) {
    if (mode.segments[s].palette[i].type == COLOR_MORPH ||
        mode.segments[s].palette[i].type == COLOR_ROTATE) {
      mode.segments[s].palette[i].timer = interp_value(mode.segments[s].palette[i].timer_v);
    }
  }
}

void accel_calc_counts_and_delta(uint8_t s) {
  mode.segments[s].strobe_count = interp_value(mode.segments[s].strobe_count_v);
  mode.segments[s].repeat_count = interp_value(mode.segments[s].repeat_count_v);
  mode.segments[s].change_count = interp_value(mode.segments[s].change_count_v);
  if (mode.segments[s].color_mode == COLOR_MODE_SHIFT) {
    mode.segments[s].shift_delta = interp_value(mode.segments[s].shift_delta_v);
  }
}

void accel_calc_strobes_and_blanks(uint8_t s) {
  mode.segments[s].strobe     = interp_value(mode.segments[s].strobe_v);
  mode.segments[s].strobe_end = interp_value(mode.segments[s].strobe_end_v);
  mode.segments[s].blank      = interp_value(mode.segments[s].blank_v);
  mode.segments[s].blank_end  = interp_value(mode.segments[s].blank_end_v);
  mode.segments[s].gap        = interp_value(mode.segments[s].gap_v);
}

void handle_accel() {
  switch (accel_tick) {
    case  0: TWADC_begin();
             TWADC_write_w(ACCEL_ADDR);
             break;
    case  1: TWADC_write((uint8_t)1);
             break;
    case  2: TWADC_begin();
             TWADC_write_r(ACCEL_ADDR);
             break;
    case  3: axis = (int16_t)TWADC_read(1) << 8;
             break;
    case  4: axis = (axis | TWADC_read(1)) >> 4;
             mag  = pow(axis, 2);
             break;
    case  5: axis = (int16_t)TWADC_read(1) << 8;
             break;
    case  6: axis = (axis | TWADC_read(1)) >> 4;
             mag += pow(axis, 2);
             break;
    case  7: axis = (int16_t)TWADC_read(1) << 8;
             break;
    case  8: axis = (axis | TWADC_read(0)) >> 4;
             mag += pow(axis, 2);
             break;
    case  9: accel_thresh    = ACC_THRESH_START;
             accel_thresh_d  = ACC_THRESH_D[accel_profile];
             accel_thresh_dd = ACC_THRESH_DD[accel_profile];
             accel_velocity  = 0;
             accel_bin       = 0;
             accel_check_bins(32);
             break;
    case 10: accel_check_bins(64);
             break;
    case 11: mode.velocity = accel_velocity;
             accel_interp();
             mode.falloff = interp_value(mode.falloff);
             mode.trigger = interp_value(mode.trigger);
             break;
    case 12: accel_calc_timers(0);
             break;
    case 13: accel_calc_counts_and_delta(0);
             break;
    case 14: accel_calc_strobes_and_blanks(0);
             break;
    case 15: if (mode.nums > 1) accel_calc_timers(1);
             break;
    case 16: if (mode.nums > 1) accel_calc_counts_and_delta(1);
             break;
    case 17: if (mode.nums > 1) accel_calc_strobes_and_blanks(1);
             break;
    case 18: if (mode.nums > 2) accel_calc_timers(2);
             break;
    case 19: if (mode.nums > 2) accel_calc_counts_and_delta(2);
             break;
    case 20: if (mode.nums > 2) accel_calc_strobes_and_blanks(2);
             break;
    case 21: if (mode.nums > 3) accel_calc_timers(3);
             break;
    case 22: if (mode.nums > 3) accel_calc_counts_and_delta(3);
             break;
    case 23: if (mode.nums > 3) accel_calc_strobes_and_blanks(3);
    default: break;
  }
  accel_tick++;
  if (accel_tick >= 40) accel_tick = 0;
}
// }}}

// {{{ handle serial
void send_handshake(void) {
  Serial.write(SER_HANDSHAKE);
  Serial.write(SER_VERSION);
  Serial.write(42);
  Serial.write(42);
}

void update_mode(uint8_t addrh, uint8_t addrl, uint8_t data) {
  uint16_t addr = (addrh << 8) + addrl;
  mode.data[addr] = data;
}

void handle_serial(void) {
  uint8_t cmd, in0, in1, in2;
  while (Serial.available() >= 4) {
    cmd = Serial.read();
    in0 = Serial.read();
    in1 = Serial.read();
    in2 = Serial.read();

    if (cmd == SER_HANDSHAKE) {
      if (in0 == SER_VERSION && in1 == in2) {
        op_state = STATE_EDIT_MODE;
        send_handshake();
      }
    } else if (cmd == SER_DISCONNECT) {
      goto_on();
    } else if (cmd == SER_INIT) {
      init_state();
    } else if (cmd == SER_VIEW_MODE) {
      op_state = STATE_EDIT_MODE;
    } else if (cmd == SER_VIEW_COLOR) {
      op_state = STATE_EDIT_COLOR;
    } else if (cmd == SER_EDIT_MODE) {
      update_mode(in0, in1, in2);
    } else if (cmd == SER_EDIT_COLOR) {
      edit_color.h = in0;
      edit_color.s = in1;
      edit_color.v = in2;
    }
  }
}
// }}}

// {{{ handle render
// {{{ render color
uint16_t calc_hue_diff(Color *c) {
  uint8_t h0 = c->colors[0].h;
  uint8_t h1 = c->colors[1].h;
  uint16_t rtn;

  if (c->dir & 0x1 == 0) {
    if (h1 <= h0) h1 += 24;
    rtn = h1 - h0;
  } else {
    if (h0 <= h1) h0 += 24;
    rtn = h0 - h1;
  }

  if (c->dir & 0x2 != 0) rtn <<= 1;
  return rtn << 3;
}

void render_color(Color *c, uint16_t rot) {
  uint8_t i, j;
  uint16_t x, d;
  if (c->type == COLOR_STATIC) {
    unhsv(&c->colors[0], rot, &frame);
  } else if (c->type == COLOR_COMPOUND) {
    unhsv(&c->colors[c->cidx], rot, &frame);
  } else if (c->type == COLOR_REACTIVE) {
    blend_f(&c->colors[mode.iv_s], &c->colors[mode.iv_e], mode.iv_f, rot, &frame);
  } else if (c->type == COLOR_MORPH) {
    i = c->cidx >> 4;
    j = i + 1;
    if (j >= c->numc) i = 0;
    blend(&c->colors[i], &c->colors[j], rot, c->counter, 100, &frame);
  } else if (c->type == COLOR_STREAK) {
    // track that color has been played
    c->counter = 1;
    i, j;
    if (c->dir == 0) {
      i = c->cidx;
      j = i + 1;
      if (j >= c->numc) j = 0;
    } else {
      j = c->cidx;
      i = j + 1;
      if (i >= c->numc) i = 0;
    }
    d = mode.seg_len * MSEG(strobe_count);
    x = mode.seg_len * mode.strobe_num;
    x += mode.trip;
    blend(&c->colors[i], &c->colors[j], rot, x, d, &frame);
  } else if (c->type == COLOR_ROTATE) {
    i = calc_hue_diff(c);
    x = c->counter;
    if (c->cidx != 0) x = 100 - x;
    rot += interp(0, i, x, 100);
    unhsv(&c->colors[0], rot, &frame);
  } else if (c->type == COLOR_RAINBOW) {
    i = calc_hue_diff(c);
    d = mode.seg_len * MSEG(strobe_count);
    x = mode.seg_len * mode.strobe_num;
    x += mode.trip;
    rot += interp(0, i, x, d);
    unhsv(&c->colors[0], rot, &frame);
  }
}

void tick_color(Color *c) {
  uint8_t r, s;
  if (c->type == COLOR_COMPOUND) {
    c->cidx++;
    if (c->cidx >= c->numc) c->cidx = 0;
  } else if (c->type == COLOR_MORPH || c->type == COLOR_ROTATE) {
    c->tick++;
    uint8_t steps = (c->type == COLOR_MORPH) ? c->numc : (c->dir & 0x2 == 0) ? 1 : 2;
    if (c->tick >= c->timer) {
      c->repeat++;
      if (c->repeat >= 10) {
        c->repeat = 0;
        c->counter++;
        if (c->counter >= 100) {
          c->counter = 0;
          c->cidx++;
          if (c->cidx >= steps) {
            c->cidx = 0;
          }
        }
      }
    }
  }
}

void tick_colors(void) {
  for (uint8_t s = 0; s < mode.nums; s++) {
    for (uint8_t c = 0; c < mode.segments[s].numc; c++) {
      tick_color(&mode.segments[s].palette[c]);
    }
  }
}
// }}}

// {{{ render pattern
uint8_t calc_step(void) {
  bool strobe = mode.part == 0;
  bool ovr = MSEG(change_mode) & 0b010;
  bool alt = MSEG(change_mode) & 0b100;

  uint8_t s = (strobe) ? MSEG(strobe)     : MSEG(blank);
  uint8_t e = (strobe) ? MSEG(strobe_end) : MSEG(blank_end);
  if (s == e) return s;

  uint8_t x =  (ovr) ? MSEG(change_num)   : mode.strobe_num;
  uint8_t d = ((ovr) ? MSEG(change_count) : MSEG(strobe_count)) - 1;
  if (!strobe) d--;
  if (x == 0) return s;
  if (d == 0) return s;
  if (x >= d) return e;

  uint8_t h;
  if (alt) {
    h = (d >> 1) + (d & 0x1);
    x = (x < h) ? x : d - x - 1;
    d = h;
    if (strobe || d & 0x1) d--;
  } else {
    d--;
  }
  return interp(s, e, x, d);
}

void render_pattern(void) {
  uint32_t f;
  uint16_t _x, x, d, h;
  uint8_t s, e;
  bool effect_dir, rot_dir;
  uint8_t c, rot;

  while (mode.trip >= mode.seg_len) {
    mode.trip = 0;
    // calculate part values/state
    // next part
    mode.part++;
    if (mode.part >= 2) {
      mode.part = 0;
      // handle basic color mode
      if (MSEG(color_mode) == COLOR_MODE_BASIC) {
        MSEG(cidx) += 1;
        while (MSEG(cidx) >= MSEG(numc)) MSEG(cidx) -= MSEG(numc);
      }
      // next strobe
      mode.strobe_num++;
      if (mode.strobe_num >= MSEG(strobe_count)) {
        mode.strobe_num = 0;
        // next segment
        // see next repeat, if we trip repeat, handle color change
        MSEG(repeat_num)++;
        if (MSEG(repeat_num) >= MSEG(repeat_count)) {
          MSEG(repeat_num) = 0;
          if (MSEG(color_mode) == COLOR_MODE_GROUP || MSEG(color_mode) == COLOR_MODE_REVERSE) {
            MSEG(cidx) = (MSEG(cmod1) < 2 || MSEG(cmod2) == CMOD2_DROP_ONE) ? 1 : MSEG(cmod1);
          } else if (MSEG(color_mode) == COLOR_MODE_SHIFT) {
            MSEG(cidx)++;
            if (MSEG(cmod1) > 1) MSEG(flip_rot)    = !MSEG(flip_rot);
            if (MSEG(cmod2) > 1) MSEG(flip_effect) = !MSEG(flip_effect);
          }
          while (MSEG(cidx) >= MSEG(numc)) MSEG(cidx) -= MSEG(numc);
        }
        // increment change num if change over
        if (MSEG(change_mode) & 0x2 != 0) {
          MSEG(change_num)++;
          if (MSEG(change_num) >= MSEG(change_count)) MSEG(change_num) = 0;
        }
        // when the segment changes, check all colors from the previous to see if they have played
        for (uint8_t cslot = 0; cslot < MSEG(numc); cslot++) {
          if (COLOR(cslot).type == COLOR_STREAK && COLOR(cslot).counter != 0) {
            COLOR(cslot).cidx++;
            if (COLOR(cslot).cidx >= COLOR(cslot).numc) COLOR(cslot).cidx = 0;
            COLOR(cslot).counter = 0;
          }
        }
        mode.segment_num++;
        if (mode.segment_num >= mode.nums) mode.segment_num = 0;
        while (MSEG(repeat_count) == 0) {
          mode.segment_num++;
          if (mode.segment_num >= mode.nums) mode.segment_num = 0;
        }
      }
    }
    // calculate part length
    if (mode.part != 0 && mode.strobe_num == MSEG(strobe_count) - 1) {
      mode.seg_len = MSEG(gap);
    } else if (MSEG(change_mode) == CHANGE_OFF) {
      mode.seg_len = (mode.part == 0) ? MSEG(strobe) : MSEG(blank);
    } else {
      mode.seg_len = calc_step();
    }
  }
  // Calculate color
  if (mode.part == 0) {
    // calculate these for dimming or for shift mode
    d = mode.seg_len * MSEG(strobe_count);
    _x = mode.seg_len * mode.strobe_num;
    // determine color to render and rotation
    if (MSEG(color_mode) == COLOR_MODE_BASIC) {
      rot = 0;
      c = MSEG(cidx);
    } else if (MSEG(color_mode) == COLOR_MODE_SHIFT) {
      c = MSEG(cidx);
      // get x through effect direction
      effect_dir = (MSEG(cmod2) & 0x1) ^ MSEG(flip_effect);
      x = (effect_dir) ? _x + mode.trip + 1 : _x + mode.seg_len - mode.trip;
      // calculate rotation
      rot = interp(0, MSEG(shift_delta), x, d);
      rot_dir = (MSEG(cmod1) & 0x1) ^ MSEG(flip_rot);
      if (rot_dir) rot = 192 - rot;
    } else {
      rot = 0;
      // calculate based on strobe num, never higher than (group - 1) (0 - 7)
      c = mode.strobe_num;
      while (c >= MSEG(cmod1)) c -= MSEG(cmod1);
      // reversed if group > 1
      if (MSEG(color_mode) == COLOR_MODE_REVERSE && MSEG(cmod1) > 1) c = (MSEG(cmod1) - 1) - c;
      c += MSEG(cidx);
      while (c >= MSEG(numc)) c -= MSEG(numc);
    }
    // render out color
    render_color(&COLOR(c), rot);
    // handle dimming
    if (MSEG(dim_mode) != DIM_MODE_OFF) {
      x = _x + mode.trip;

      if (MSEG(dim_mode) == DIM_MODE_FADE_OUT) {
        x = d - 1 - x;
      } else if (MSEG(dim_mode) == DIM_MODE_PULSE) {
        h = d >> 1;
        if (x <= h) {
          d = h;
        } else {
          x = (d - 1) - (x - h);
          d = d - h;
        }
      }

      f = get_f(x, d);
      frame.r = interp_f(0, frame.r, f);
      frame.g = interp_f(0, frame.g, f);
      frame.b = interp_f(0, frame.b, f);
    }
  }
}
// }}}

void handle_render(void) {
  frame.r = frame.g = frame.b = 0;

  if (flash_ticks > 0) {
    if (flash_tick == 0)  flash_tick = 200;
    if (flash_tick > 100) hue_color(flash_color, &frame);
    flash_tick--;
    flash_ticks--;
  } else {
    if (op_state == STATE_ON || op_state == STATE_EDIT_MODE) {
      if (!was_pressed) render_pattern();
      tick_colors();
    } else if (op_state == STATE_EDIT_COLOR) {
      unhsv(&edit_color, 0, &frame);
    }
  }
}
// }}}

// {{{ handle button
void goto_on(void) {
  op_state = STATE_ON;
  mode_idx = 0;
  load_mode();
}

void handle_button(void) {
  bool pressed = digitalRead(PIN_BUTTON) == LOW;
  bool changed = pressed != was_pressed;

  if (op_state == STATE_OFF) {
    if (pressed) {
      if (since_press == 6000) flash(0);
    } else if (changed) {
      if (since_press >= 6000) {
        EEPROM.update(ADDR_LOCKED, 1);
        enter_sleep();
      } else {
        goto_on();
      }
    }
  } else if (op_state == STATE_LOCKED) {
    if (pressed) {
      if (since_press == 6000) flash(8);
    } else if (changed) {
      if (since_press >= 6000) {
        EEPROM.update(ADDR_LOCKED, 0);
        goto_on();
      } else {
        enter_sleep();
      }
    }
  } else if (op_state == STATE_ON) {
    if (pressed) {
    } else if (changed) {
      if (since_press < 700) {
        mode_idx++;
        if (mode_idx >= NUM_MODES) mode_idx = 0;
        load_mode();
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

// {{{ setup and loop
void setup(void) {
  pinMode(PIN_BUTTON, INPUT);
  power_down();

  ADCSRA = 0b10000100;                            // ADC enabled @ x16 prescaler
  pinMode(PIN_LDO, OUTPUT);                       // Enable accel pwr pin
  digitalWrite(PIN_LDO, HIGH);                    // Power on accel
  pinMode(PIN_R, OUTPUT);                         // Enable LED pins for output
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  noInterrupts();                                 // Configure timers for fastest PWM
  TCCR0B = (TCCR0B & 0b11111000) | 0b001;         // no prescaler ~64/ms
  TCCR1B = (TCCR1B & 0b11111000) | 0b001;         // no prescaler ~32/ms
  sbi(TCCR1B, WGM12);                             // fast PWM ~64/ms
  interrupts();
  accel_init();                                   // Initialize the accelerometer
  wdt_enable(WDTO_15MS);                          // Enable the watchdog
  Serial.begin(115200);                           // Init serial connection
  send_handshake();

  // init light states
  op_state = EEPROM.read(ADDR_LOCKED) ? STATE_LOCKED : STATE_OFF;
  load_mode();
  accel_tick = 0;
  last_write = micros();
}

void loop(void) {
  handle_serial();
  handle_button();
  handle_accel();
  handle_render();

  uint32_t cus = micros();
  while (cus - last_write < LIMITER_US) cus = micros();
  last_write = cus;

  analogWrite(PIN_R, frame.r);
  analogWrite(PIN_G, frame.g);
  analogWrite(PIN_B, frame.b);

  wdt_reset();
}
// }}}
