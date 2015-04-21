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

#ifndef UTIL_H
#define UTIL_H

#include <sstream>

template <typename T> struct Identity
{
  T operator()(T &&t) { return t; }
};

template <typename L, typename Callback = Identity<decltype(*L().begin())>>
std::string listToString(
  L &list, Callback callback = Identity<decltype(*list.begin())>())
{
  std::stringstream res;
  bool first = true;
  for (auto &&e : list)
  {
    if (first)
      first = false;
    else
      res << ", ";
    res << callback(std::forward<decltype(e)>(e));
  }
  return res.str();
}

#endif // UTIL_H