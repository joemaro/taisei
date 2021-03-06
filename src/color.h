/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2017, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2017, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once

#include <stdint.h>
#include "taiseigl.h"
#include "util.h"

#define CLR_R ((Color)48)
#define CLR_G ((Color)32)
#define CLR_B ((Color)16)
#define CLR_A ((Color)0)

#define CLR_CMASK ((Color)0xffff)
#define CLR_ONEVALUE ((Color)0xff)

#define CLRMASK_R (CLR_CMASK << CLR_R)
#define CLRMASK_G (CLR_CMASK << CLR_G)
#define CLRMASK_B (CLR_CMASK << CLR_B)
#define CLRMASK_A (CLR_CMASK << CLR_A)

typedef uint64_t Color;
typedef int16_t ColorComponent;

Color rgba(float r, float g, float b, float a) __attribute__((const));
Color rgb(float r, float g, float b) __attribute__((const));
Color hsla(float h, float s, float l, float a) __attribute__((const));
Color hsl(float h, float s, float l) __attribute__((const));

void parse_color(Color clr, float *r, float *g, float *b, float *a);
void parse_color_call(Color clr, tsglColor4f_ptr func);
void parse_color_array(Color clr, float array[4]);
Color derive_color(Color src, Color mask, Color mod) __attribute__((const));
Color multiply_colors(Color c1, Color c2) __attribute__((const));
Color add_colors(Color c1, Color c2) __attribute__((const));
Color subtract_colors(Color c1, Color c2) __attribute__((const));
Color mix_colors(Color c1, Color c2, double a) __attribute__((const));
Color approach_color(Color src, Color dst, double delta) __attribute__((const));
float color_component(Color clr, unsigned int ofs) __attribute__((const));

int color_test(void);
