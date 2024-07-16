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
// Created by Longda on 2021/4/19.
//

#ifndef __COMMON_METRICS_METRICS_H__
#define __COMMON_METRICS_METRICS_H__

#include <atomic>
#include <cstdint>
#include <sys/time.h>
#include <sys/types.h>
#include "common/metrics/metric.h"
#include "common/metrics/snapshot.h"
#include "common/metrics/uniform_reservoir.h"

namespace common {

class Gauge : public Metric
{
public:
  // user implement snapshot function
  void set_snapshot(Snapshot *value) { snapshot_value_ = value; }
};

class Counter : public Metric
{
  void set_snapshot(SnapshotBasic<int64_t> *value) { snapshot_value_ = value; }
};

class Meter : public Metric
{
public:
  Meter();
  virtual ~Meter();

  void inc(int64_t increase);
  void inc();

  void snapshot() override;

protected:
  std::atomic<int64_t> value_;
  int64_t              snapshot_tick_;
};

// SimpleTimer just get tps and meanvalue
// time unit is ms
class SimpleTimer : public Meter
{
public:
  ~SimpleTimer() override;

  void inc(int64_t increase);

  void update(int64_t one);

  void snapshot() override;

protected:
  std::atomic<int64_t> times_;
};

// Histogram metric is complicated, in normal case ,
//  please skip us histogram or Timer as more as possible
//  try use SimpleTimer to replace them.
//  if use histogram , please use sampling method.
class Histogram : public UniformReservoir
{
public:
  explicit Histogram(RandomGenerator &random);
  Histogram(RandomGenerator &random, size_t size);
  ~Histogram() override;

  void snapshot() override;
};

// timeunit is ms
// Timer = Histogram + Meter
class Timer : public UniformReservoir
{
public:
  explicit Timer(RandomGenerator &random);
  Timer(RandomGenerator &random, size_t size);
  ~Timer() override;

  void snapshot() override;
  void update(double ms) override;

protected:
  std::atomic<int64_t> value_;
  int64_t              snapshot_tick_;
};
// update ms
class TimerStat
{
public:
  explicit TimerStat(SimpleTimer &other_st);

  ~TimerStat();
  void start();
  void end();

  SimpleTimer &st_;
  int64_t      start_tick_;
  int64_t      end_tick_;
};

}  // namespace common
#endif  //__COMMON_METRICS_METRICS_H__
