// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once

#include <iostream>
#include <type_traits>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <vector>

#include <thrust/host_vector.h>

std::vector<size_t> get_sizes()
{
  std::vector<size_t> sizes = {
    0, 1, 2, 12, 63, 64, 211, 256, 344,
    1024, 2048, 5096, 34567, (1 << 17) - 1220, 1000000, (1 << 20) - 123
  };
  return sizes;
}

template<class T>
inline auto get_random_data(size_t size, T min, T max)
  -> typename std::enable_if<rocprim::is_integral<T>::value, thrust::host_vector<T>>::type
{
  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_int_distribution<T> distribution(min, max);
  thrust::host_vector<T> data(size);
  std::generate(data.begin(), data.end(), [&]() { return distribution(gen); });
  return data;
}

template<class T>
inline auto get_random_data(size_t size, T min, T max)
  -> typename std::enable_if<rocprim::is_floating_point<T>::value, thrust::host_vector<T>>::type
{
  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_real_distribution<T> distribution(min, max);
  thrust::host_vector<T> data(size);
  std::generate(data.begin(), data.end(), [&]() { return distribution(gen); });
  return data;
}

template<class T>
struct custom_compare_less
{
  __host__ __device__
  bool operator()(const T &lhs, const T &rhs) const
  {
    return lhs < rhs;
  }
}; // end less