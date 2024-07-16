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
// Created by int64_tda on 2021/4/19.
//

#include "common/metrics/metrics.h"
#include "common/metrics/timer_snapshot.h"
#include "common/lang/mutex.h"
#include <sys/types.h>

namespace common {
Meter::Meter()
{
  struct timeval start_time;
  gettimeofday(&start_time, nullptr);

  snapshot_tick_ = start_time.tv_sec * 1000000 + start_time.tv_usec;
  value_.store(0L);
}

Meter::~Meter()
{
  if (snapshot_value_ != nullptr) {
    delete snapshot_value_;
    snapshot_value_ = nullptr;
  }
}

void Meter::inc(int64_t increase) { value_.fetch_add(increase); }

void Meter::inc() { inc(1L); }

void Meter::snapshot()
{
  // lock here

  struct timeval now;
  gettimeofday(&now, nullptr);

  u_int64_t now_tick = now.tv_sec * 1000000 + now.tv_usec;

  double temp_value = (static_cast<double>(value_.exchange(0L))) / ((now_tick - snapshot_tick_) / 1000000.0);
  snapshot_tick_    = now_tick;

  if (snapshot_value_ == nullptr) {
    snapshot_value_ = new SnapshotBasic<double>();
  }
  (static_cast<SnapshotBasic<double> *>(snapshot_value_))->setValue(temp_value);
}

SimpleTimer::~SimpleTimer()
{
  if (snapshot_value_ != nullptr) {
    delete snapshot_value_;
    snapshot_value_ = nullptr;
  }
}

void SimpleTimer::inc(int64_t increase)
{
  value_.fetch_add(increase);
  times_.fetch_add(1);
}

void SimpleTimer::update(int64_t one) { inc(one); }

void SimpleTimer::snapshot()
{

  // lock here
  struct timeval now;
  gettimeofday(&now, nullptr);

  int64_t now_tick = now.tv_sec * 1000000 + now.tv_usec;

  int64_t value_snapshot = value_.exchange(0L);
  int64_t times_snapshot = times_.exchange(0L);

  double tps  = 0;
  double mean = 0;

  if (times_snapshot > 0) {
    tps  = (static_cast<double>(times_snapshot)) / ((now_tick - snapshot_tick_) / 1000000.0);
    mean = (static_cast<double>(value_snapshot)) / times_snapshot;
  }

  snapshot_tick_ = now_tick;

  if (snapshot_value_ == nullptr) {
    snapshot_value_ = new SimplerTimerSnapshot();
  }
  (static_cast<SimplerTimerSnapshot *>(snapshot_value_))->setValue(mean, tps);
}

Histogram::Histogram(RandomGenerator &random) : UniformReservoir(random) {}

Histogram::Histogram(RandomGenerator &random, size_t size) : UniformReservoir(random, size) {}

Histogram::~Histogram() = default;

void Histogram::snapshot() { UniformReservoir::snapshot(); }

Timer::Timer(RandomGenerator &random) : UniformReservoir(random)
{
  struct timeval start_time;
  gettimeofday(&start_time, nullptr);

  snapshot_tick_ = start_time.tv_sec * 1000000 + start_time.tv_usec;
  value_.store(0L);
}

Timer::Timer(RandomGenerator &random, size_t size) : UniformReservoir(random, size)
{
  struct timeval start_time;
  gettimeofday(&start_time, nullptr);

  snapshot_tick_ = start_time.tv_sec * 1000000 + start_time.tv_usec;
  value_.store(0L);
}

Timer::~Timer()
{
  if (snapshot_value_ == nullptr) {
    delete snapshot_value_;
    snapshot_value_ = nullptr;
  }
}

void Timer::update(double ms)
{
  UniformReservoir::update(ms);
  value_.fetch_add(1L);
}

void Timer::snapshot()
{
  if (snapshot_value_ == nullptr) {
    snapshot_value_ = new TimerSnapshot();
  }
  auto *timer_snapshot = static_cast<TimerSnapshot *>(snapshot_value_);

  struct timeval now;
  gettimeofday(&now, nullptr);

  int64_t now_tick = now.tv_sec * 1000000 + now.tv_usec;

  double tps     = (static_cast<double>(value_.exchange(0L))) / ((now_tick - snapshot_tick_) / 1000000.0);
  snapshot_tick_ = now_tick;

  MUTEX_LOCK(&mutex);
  std::vector<double> output = data;
  MUTEX_UNLOCK(&mutex);

  timer_snapshot->set_collection(output);
  timer_snapshot->set_tps(tps);
}

TimerStat::TimerStat(SimpleTimer &other_st) : st_(other_st), start_tick_(0), end_tick_(0) { start(); }

TimerStat::~TimerStat()
{
  if (end_tick_ == 0) {
    end();
  }

  st_.update((end_tick_ - start_tick_) / 1000);
}

void TimerStat::start()
{
  struct timeval now;
  gettimeofday(&now, nullptr);

  start_tick_ = now.tv_sec * 1000000 + now.tv_usec;
}

void TimerStat::end()
{
  struct timeval now;
  gettimeofday(&now, nullptr);

  end_tick_ = now.tv_sec * 1000000 + now.tv_usec;
}

}  // namespace common