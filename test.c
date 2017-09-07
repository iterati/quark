#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define CONSTRAIN(x, n, m) (((x) < (n)) ? (n) : (((x) > (m)) ? (m) : (x)))

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

const uint8_t CURVE64[9] = {0, 1, 4, 9, 16, 25, 36, 49, 64};

uint8_t norm64(uint8_t x, uint8_t d) {
    if (x >= d)
        return 64;

    while (d < 64) {
        d <<= 1;
        x <<= 1;
    }

    uint8_t v = x;
    uint8_t t = d;
    uint8_t r = 0;

    uint8_t m = 0x20;

    while (m) {
        //t = (t + 1) >> 1;
        t >>= 1;
        if (v >= t) {
            v -= t;
            r |= m;
        }
        m >>= 1;
    }

    return r;
}

uint8_t interp64(uint8_t s, uint8_t e, uint8_t n) {
    if (n == 0)  return s;
    if (n >= 64) return e;

    uint8_t r = 0;
    uint8_t d = (s < e) ? e - s : s - e;
    uint8_t m = 0x20;
    uint8_t a;

    while (m && d) {
        a = d & 0x01;
        d >>= 1;
        if (n & m)
            r += d + a;

        m >>= 1;
    }

    return r;
}

//uint8_t norm64(uint8_t x, uint8_t d) {
//    if (x >= d) {
//        return 64;
//    }
//
//    while (d < 64) {
//        d <<= 1;
//        x <<= 1;
//    }
//
//    uint8_t v = x; // value we are normalizing
//    uint8_t t = d; // value we are tracking currently
//    uint8_t r = 0; // normalized value to return
//
//    t >>= 1;
//    if (v >= t) {
//        v -= t;
//        r |= 0x20;
//    }
//
//    t >>= 1;
//    if (v >= t) {
//        v -= t;
//        r |= 0x10;
//    }
//
//    t >>= 1;
//    if (v >= t) {
//        v -= t;
//        r |= 0x08;
//    }
//
//    t >>= 1;
//    if (v >= t) {
//        v -= t;
//        r |= 0x04;
//    }
//
//    t >>= 1;
//    if (v >= t) {
//        v -= t;
//        r |= 0x02;
//    }
//
//    t >>= 1;
//    if (v >= t) {
//        v -= t;
//        r |= 0x01;
//    }
//
//    return r;
//}

//uint8_t interp64(uint8_t s, uint8_t e, uint8_t n) {
//    if (n == 0)  return s;
//    if (n >= 64) return e;
//
//    uint8_t r = 0;
//    uint8_t d = (s < e) ? e - s : s - e;
//    uint8_t a;
//    
//    a = d & 0x01;
//    d >>= 1;
//    if (n & 0x20)
//        r += d + a;
//
//    a = d & 0x01;
//    d >>= 1;
//    if (n & 0x10)
//        r += d + a;
//
//    a = d & 0x01;
//    d >>= 1;
//    if (n & 0x08)
//        r += d + a;
//
//    a = d & 0x01;
//    d >>= 1;
//    if (n & 0x04)
//        r += d + a;
//
//    a = d & 0x01;
//    d >>= 1;
//    if (n & 0x02)
//        r += d + a;
//
//    a = d & 0x01;
//    d >>= 1;
//    if (n & 0x01)
//        r += d + a;
//
//    return (s < e) ? s + r : s - r;
//}

uint8_t interp(uint8_t s, uint8_t e, uint8_t x, uint8_t d) {
    uint8_t n = norm64(x, d);
    return interp64(s, e, n);
}

uint8_t interp_v(uint8_t v, uint8_t s, uint8_t e, uint8_t n, uint8_t m) {
    uint8_t x = (v < n) ? n : (v > m) ? m : v;
    uint8_t d = m - n;
    uint8_t nx = norm64(v, d);
    return interp64(s, e, nx);
}


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


int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Missing arguments\n");
        return 1;
    }

    uint8_t s = atoi(argv[1]);
    uint8_t e = atoi(argv[2]);
    uint8_t d = atoi(argv[3]);

    for (int x = 0; x <= d; x++) {
        printf("%d:\t%d\t%d\n", x, norm64(x, d), interp(s, e, x, d));
    }

    return 0;
}
