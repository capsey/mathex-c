/*
  Copyright (c) 2023 Caps Lock

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "mx_token.h"
#include <math.h>

static double internal_add(double a, double b) { return a + b; }
static double internal_sub(double a, double b) { return a - b; }
static double internal_mul(double a, double b) { return a * b; }
static double internal_div(double a, double b) { return a / b; }

const mx_token builtin_add = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_add, .prec = 2, .lassoc = true}};
const mx_token builtin_sub = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_sub, .prec = 2, .lassoc = true}};
const mx_token builtin_mul = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_mul, .prec = 3, .lassoc = true}};
const mx_token builtin_div = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = internal_div, .prec = 3, .lassoc = true}};

const mx_token builtin_pow = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = pow, .prec = 4, .lassoc = false}};
const mx_token builtin_mod = {.type = MX_BINARY_OPERATOR, .d.biop = {.call = fmod, .prec = 3, .lassoc = true}};

static double internal_pos(double a) { return +a; }
static double internal_neg(double a) { return -a; }

const mx_token builtin_pos = {.type = MX_UNARY_OPERATOR, .d.unop = internal_pos};
const mx_token builtin_neg = {.type = MX_UNARY_OPERATOR, .d.unop = internal_neg};
