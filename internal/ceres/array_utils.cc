// Ceres Solver - A fast non-linear least squares minimizer
// Copyright 2023 Google Inc. All rights reserved.
// http://ceres-solver.org/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: sameeragarwal@google.com (Sameer Agarwal)

#include "ceres/array_utils.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

#include "absl/strings/str_format.h"
#include "ceres/types.h"

namespace ceres::internal {

bool IsArrayValid(const int64_t size, const double* x) {
  if (x != nullptr) {
    for (int64_t i = 0; i < size; ++i) {
      if (!std::isfinite(x[i]) || (x[i] == kImpossibleValue)) {
        return false;
      }
    }
  }
  return true;
}

int64_t FindInvalidValue(const int64_t size, const double* x) {
  if (x == nullptr) {
    return size;
  }

  for (int64_t i = 0; i < size; ++i) {
    if (!std::isfinite(x[i]) || (x[i] == kImpossibleValue)) {
      return i;
    }
  }

  return size;
}

void InvalidateArray(const int64_t size, double* x) {
  if (x != nullptr) {
    for (int64_t i = 0; i < size; ++i) {
      x[i] = kImpossibleValue;
    }
  }
}

void AppendArrayToString(const int64_t size,
                         const double* x,
                         std::string* result) {
  for (int64_t i = 0; i < size; ++i) {
    if (x == nullptr) {
      absl::StrAppendFormat(result, "Not Computed  ");
    } else {
      if (x[i] == kImpossibleValue) {
        absl::StrAppendFormat(result, "Uninitialized ");
      } else {
        absl::StrAppendFormat(result, "%12g ", x[i]);
      }
    }
  }
}

void MapValuesToContiguousRange(const int64_t size, int* array) {
  std::vector<int> unique_values(array, array + size);
  std::sort(unique_values.begin(), unique_values.end());
  unique_values.erase(std::unique(unique_values.begin(), unique_values.end()),
                      unique_values.end());

  for (int64_t i = 0; i < size; ++i) {
    array[i] =
        std::lower_bound(unique_values.begin(), unique_values.end(), array[i]) -
        unique_values.begin();
  }
}

}  // namespace ceres::internal
