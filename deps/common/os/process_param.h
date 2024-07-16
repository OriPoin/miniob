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
// Created by Longda on 2010
//

#pragma once

#include "common/lang/string.h"
#include "common/lang/vector.h"

namespace common {

class ProcessParam
{

public:
  ProcessParam() = default;

  virtual ~ProcessParam() = default;

  void init_default(string &process_name);

  [[nodiscard]] const string &get_std_out() const { return std_out_; }

  void set_std_out(const string &std_out) { ProcessParam::std_out_ = std_out; }

  [[nodiscard]] const string &get_std_err() const { return std_err_; }

  void set_std_err(const string &std_err) { ProcessParam::std_err_ = std_err; }

  [[nodiscard]] const string &get_conf() const { return conf; }

  void set_conf(const string &conf) { ProcessParam::conf = conf; }

  [[nodiscard]] const string &get_process_name() const { return process_name_; }

  void set_process_name(const string &processName) { ProcessParam::process_name_ = processName; }

  [[nodiscard]] bool is_demon() const { return demon; }

  void set_demon(bool demon) { ProcessParam::demon = demon; }

  [[nodiscard]] const vector<string> &get_args() const { return args; }

  void set_args(const vector<string> &args) { ProcessParam::args = args; }

  void set_server_port(int port) { server_port_ = port; }

  [[nodiscard]] int get_server_port() const { return server_port_; }

  void set_unix_socket_path(const char *unix_socket_path) { unix_socket_path_ = unix_socket_path; }

  [[nodiscard]] const string &get_unix_socket_path() const { return unix_socket_path_; }

  void set_protocol(const char *protocol) { protocol_ = protocol; }

  [[nodiscard]] const string &get_protocol() const { return protocol_; }

  void set_trx_kit_name(const char *kit_name)
  {
    if (kit_name) {
      trx_kit_name_ = kit_name;
    }
  }
  [[nodiscard]] const string &trx_kit_name() const { return trx_kit_name_; }

  void set_thread_handling_name(const char *thread_handling_name)
  {
    if (thread_handling_name) {
      thread_handling_name_ = thread_handling_name;
    }
  }

  [[nodiscard]] const string &thread_handling_name() const { return thread_handling_name_; }

  void set_buffer_pool_memory_size(int bytes) { buffer_pool_memory_size_ = bytes; }

  [[nodiscard]] int buffer_pool_memory_size() const { return buffer_pool_memory_size_; }

  void                        set_durability_mode(const char *mode) { durability_mode_ = mode; }
  [[nodiscard]] const string &durability_mode() const { return durability_mode_; }

private:
  string         std_out_;           // The output file
  string         std_err_;           // The err output file
  string         conf;               // The configuration file
  string         process_name_;      // The process name
  bool           demon = false;      // whether demon or not
  vector<string> args;               // arguments
  int            server_port_ = -1;  // server port(if valid, will overwrite the port in the config file)
  string         unix_socket_path_;
  string         protocol_;
  string         trx_kit_name_;
  string         thread_handling_name_;
  int            buffer_pool_memory_size_ = -1;
  string         durability_mode_;
};

ProcessParam *&the_process_param();

}  // namespace common
