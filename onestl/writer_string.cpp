/*
 * onestl - tiny STL C++ library
 *
 * Copyright (C) 2018-2021 Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * writer_string.cpp
 *
 *  Created on Jan 26, 2021
 *      Author Massimiliano Ghilardi
 */

#include <onestl/string.hpp>
#include <onestl/writer.hpp>
#include <onestl/writer_string.hpp>

#include <cerrno> // ENOMEM

namespace onestl {

static int write_to_string(void *handle, const char *chars, size_t n) noexcept {
  String *dst = reinterpret_cast<String *>(handle);
  int err = 0;
  if (!dst) {
    err = EINVAL;
  } else if (!dst->append(View<char>{chars, n})) {
    err = ENOMEM;
  }
  return err;
}

template <> Writer Writer::make<String *>(String *dst) noexcept {
  return Writer{dst, write_to_string};
}

} // namespace onestl