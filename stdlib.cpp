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

#include "ast.h"

#include <iostream>

void prt(types::int_t i) { std::cout << i; }
void prtln(types::int_t i) { std::cout << i << std::endl; }

void prt(types::flt_t f) { std::cout << f; }
void prtln(types::flt_t f) { std::cout << f << std::endl; }

void prt(types::chr_t c) { std::cout << c; }
void prtln(types::chr_t c) { std::cout << c << std::endl; }

void prt(types::boo_t b) { std::cout << b; }
void prtln(types::boo_t b) { std::cout << b << std::endl; }

void prt(types::str_t s) { std::cout << s; }
void prtln(types::str_t s) { std::cout << s << std::endl; }
