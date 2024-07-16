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
// Created by Longda on 2021/4/20.
//

#include "histogram_snapshot.h"

#include <cmath>
#include <cstdio>

#include <algorithm>
#include <numeric>
#include <sstream>

namespace common {

HistogramSnapShot::HistogramSnapShot() = default;

HistogramSnapShot::HistogramSnapShot(const std::vector<double> &collection) { set_collection(collection); }

HistogramSnapShot::~HistogramSnapShot() = default;

void HistogramSnapShot::set_collection(const std::vector<double> &collection)
{
  if (collection.empty()) {
    return;
  }

  data_ = collection;
  std::sort(data_.begin(), data_.end());
}

size_t HistogramSnapShot::size() const { return data_.size(); }

double HistogramSnapShot::get_value(double quantile)
{
  if (quantile > 1.0F) {
    quantile = 1.0F;
  }

  if (quantile < 0.0F) {
    quantile = 0.0F;
  }

  if (data_.empty()) {
    return 0.0F;
  }

  double pos = quantile * (data_.size() + 1);

  if (pos < 1) {
    return data_[0];
  }

  if (pos >= data_.size()) {
    return data_[data_.size() - 1];
  }

  double lower = data_[static_cast<int>(pos) - 1];
  double upper = data_[static_cast<int>(pos)];

  return lower + (pos - floor(pos)) * (upper - lower);
}

double HistogramSnapShot::get_median() { return get_value(0.5F); }

double HistogramSnapShot::get_75th() { return get_value(0.75F); }

double HistogramSnapShot::get_90th() { return get_value(0.90F); }

double HistogramSnapShot::get_95th() { return get_value(0.95F); }
double HistogramSnapShot::get_99th() { return get_value(0.99F); }
double HistogramSnapShot::get_999th() { return get_value(0.999F); }

double HistogramSnapShot::get_max()
{
  if (data_.empty()) {
    return 0.0F;
  }

  return static_cast<double>(*data_.rbegin());
}

double HistogramSnapShot::get_min()
{
  if (data_.empty()) {
    return 0.0F;
  }

  return static_cast<double>(*data_.begin());
}

double HistogramSnapShot::get_mean()
{
  if (data_.empty()) {
    return 0.0F;
  }

  return std::accumulate(data_.begin(), data_.end(), static_cast<double>(0)) * 1.0F / data_.size();
}

const std::vector<double> &HistogramSnapShot::get_values() { return data_; }

std::string HistogramSnapShot::to_string()
{
  std::stringstream oss;
  oss << "mean:" << get_mean() << ",min:" << get_min() << ",max:" << get_max() << ",median:" << get_median()
      << ", 75th:" << get_75th() << ",90th:" << get_90th() << ",99th:" << get_99th() << ",999th:" << get_999th();

  return oss.str();
}

}  // namespace common