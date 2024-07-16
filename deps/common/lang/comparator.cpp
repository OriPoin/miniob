/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by wangyunlai on 2021/6/11.
//

#include "common/defs.h"
#include <cstring>

#include "common/lang/algorithm.h"

namespace common {

int compare_int(const char *arg1, const char *arg2)
{
  int v1 = *static_cast<const int *>(static_cast<const void *>(arg1));
  int v2 = *static_cast<const int *>(static_cast<const void *>(arg2));
  if (v1 > v2) {
    return 1;
  }
  if (v1 < v2) {
    return -1;
  }
  return 0;
}

int compare_float(const char *arg1, const char *arg2)
{
  float v1  = *static_cast<const float *>(static_cast<const void *>(arg1));
  float v2  = *static_cast<const float *>(static_cast<const void *>(arg2));
  float cmp = v1 - v2;
  if (cmp > EPSILON) {
    return 1;
  }
  if (cmp < -EPSILON) {
    return -1;
  }
  return 0;
}

int compare_string(const char *arg1, size_t arg1_max_length, const char *arg2, size_t arg2_max_length)
{
  size_t maxlen = min(arg1_max_length, arg2_max_length);
  size_t result = strncmp(arg1, arg2, maxlen);
  if (0 != result) {
    return result;
  }

  if (arg1_max_length > maxlen) {
    return arg1[maxlen] - 0;
  }

  if (arg2_max_length > maxlen) {
    return 0 - arg2[maxlen];
  }
  return 0;
}

}  // namespace common
