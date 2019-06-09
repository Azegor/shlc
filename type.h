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

#ifndef TYPE_H
#define TYPE_H

#include <string>

enum class BuiltinTypeKind : int
{
  none,
  vac_t,
  int_t,
  flt_t,
  chr_t,
  boo_t,
  str_t,
  cls_t,
};

namespace types
{
using int_t = long long;
// using flt_t = long double;
using flt_t = double;
using chr_t = signed char;
using boo_t = bool;
// using str_t = std::string;
using str_t = char const *;
}

class Type;

const std::string &getTypeName(BuiltinTypeKind t);
const std::string &getMangleName(BuiltinTypeKind t);
std::string getMangleName(Type *t);

#endif // TYPE_H
