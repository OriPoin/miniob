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
// Created by Wangyunlai 2023/6/27
//

#pragma once

#include "common/rc.h"
#include <cstddef>
#include <string>

/**
 * @brief 属性的类型
 *
 */
enum class AttrType
{
  UNDEFINED,
  CHARS,     ///< 字符串类型
  INTS,      ///< 整数类型(4字节)
  FLOATS,    ///< 浮点数类型(4字节)
  DATES,     ///< 日期类型(4字节)
  BOOLEANS,  ///< boolean类型，当前不是由parser解析出来的，是程序内部使用的
};

const char *attr_type_to_string(AttrType type);
AttrType    attr_type_from_string(const char *s);

/**
 * @brief 属性的值
 *
 */
class Value final
{
public:
  Value() = default;

  Value(AttrType attr_type, char *data, size_t length = 4) : attr_type_(attr_type) { this->set_data(data, length); }

  explicit Value(int val);
  explicit Value(float val);
  explicit Value(bool val);
  explicit Value(const char *s, size_t len = 0);

  Value(const Value &other)            = default;
  Value &operator=(const Value &other) = default;

  void set_type(AttrType type) { this->attr_type_ = type; }
  void set_data(char *data, size_t length);
  void set_data(const char *data, size_t length) { this->set_data(const_cast<char *>(data), length); }
  void set_int(int val);
  void set_float(float val);
  void set_boolean(bool val);
  void set_string(const char *s, size_t len = 0);
  void set_date(const char *s, size_t len = 0);
  void set_value(const Value &value);

  /**
   * type cast
   *
   */
  [[nodiscard]] std::string to_string() const;
  static RC                 type_cast(Value &value, AttrType dest_type, AttrType src_type);

  [[nodiscard]] int  compare(const Value &other) const;
  [[nodiscard]] int  wildcard(const Value &other) const;
  [[nodiscard]] bool valid() const;

  [[nodiscard]] const char *data() const;
  [[nodiscard]] size_t      length() const { return length_; }

  [[nodiscard]] AttrType attr_type() const { return attr_type_; }

  /**
   * 获取对应的值
   * 如果当前的类型与期望获取的类型不符，就会执行转换操作
   */
  [[nodiscard]] int         get_int() const;
  [[nodiscard]] float       get_float() const;
  [[nodiscard]] std::string get_string() const;
  [[nodiscard]] bool        get_boolean() const;

private:
  AttrType attr_type_ = AttrType::UNDEFINED;
  size_t   length_    = 0;

  union
  {
    int   int_value_;
    float float_value_;
    bool  bool_value_;
  } num_value_;
  std::string str_value_;
};