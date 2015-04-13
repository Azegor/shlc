/*
 * Copyright 2014 Azegor
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "stdlib.h"

#include "type.h"

#include <cstdio>
extern "C" {

void prt_i(types::int_t i) { std::printf("%lld", i); }
void prt_w_i(types::int_t i, types::int_t wdth)
{
  std::printf("%*lld", (int)wdth, i);
}
void prtln_i(types::int_t i) { std::printf("%lld\n", i); }

void prt_f(types::flt_t f) { std::printf("%Lf", f); }
void prt_w_f(types::flt_t f, types::int_t wdth)
{
  std::printf("%*Lf", (int)wdth, f);
}
void prtln_f(types::flt_t f) { std::printf("%Lf\n", f); }

void prt_c(types::chr_t c) { std::printf("%c", c); }
void prtln_c(types::chr_t c) { std::printf("%c\n", c); }

void prt_b(types::boo_t b) { std::printf("%s", b ? "true" : "false"); }
void prtln_b(types::boo_t b) { std::printf("%s\n", b ? "true" : "false"); }

void prt_s(types::str_t s) { std::printf("%s", s); }
void prtln_s(types::str_t s) { std::printf("%s\n", s); }
}
