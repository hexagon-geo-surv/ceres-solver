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
//
// A simple example of optimizing a sampled function by using cubic
// interpolation.

#include "absl/log/initialize.h"
#include "ceres/ceres.h"
#include "ceres/cubic_interpolation.h"

using Interpolator = ceres::CubicInterpolator<ceres::Grid1D<double>>;

// A simple cost functor that interfaces an interpolated table of
// values with automatic differentiation.
struct InterpolatedCostFunctor {
  explicit InterpolatedCostFunctor(const Interpolator& interpolator)
      : interpolator(interpolator) {}

  template <typename T>
  bool operator()(const T* x, T* residuals) const {
    interpolator.Evaluate(*x, residuals);
    return true;
  }

  static ceres::CostFunction* Create(const Interpolator& interpolator) {
    return new ceres::AutoDiffCostFunction<InterpolatedCostFunctor, 1, 1>(
        interpolator);
  }

 private:
  const Interpolator& interpolator;
};

int main(int argc, char** argv) {
  absl::InitializeLog();
  // Evaluate the function f(x) = (x - 4.5)^2;
  const int kNumSamples = 10;
  double values[kNumSamples];
  for (int i = 0; i < kNumSamples; ++i) {
    values[i] = (i - 4.5) * (i - 4.5);
  }

  ceres::Grid1D<double> array(values, 0, kNumSamples);
  Interpolator interpolator(array);

  double x = 1.0;
  ceres::Problem problem;
  ceres::CostFunction* cost_function =
      InterpolatedCostFunctor::Create(interpolator);
  problem.AddResidualBlock(cost_function, nullptr, &x);

  ceres::Solver::Options options;
  options.minimizer_progress_to_stdout = true;
  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);
  std::cout << summary.BriefReport() << "\n";
  std::cout << "Expected x: 4.5. Actual x : " << x << std::endl;
  return 0;
}
