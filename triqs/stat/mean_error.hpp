/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : P. Dumitrescu, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#pragma once

#include <itertools/itertools.hpp>
#include <mpi/vector.hpp>
#include <triqs/arrays.hpp>

namespace triqs::stat {
  /// Calculate the arithmetic mean [REF] of data in an iterable container.
  /// @tparam V iterable type. Pre-conditions: elements must be addable to each other
  /// @param data Container with data
  /// @return Mean of data; type is deduced from first element of :param:`data`
  /// @example triqs/stat/mean_1.cpp
  /// @brief Calculate mean
  template <typename V> auto mean(V const &data) {
    // ENSURE(data.size() > 0);
    auto mean_calc = data[0];
    mean_calc      = 0;
    for (auto const &[n, x] : itertools::enumerate(data)) { mean_calc += (x - mean_calc) / (n + 1); }
    return mean_calc;
  }

  /// Calculate the arithmetic mean [REF] of data spread over multiple MPI threads. Reduces the answer to all threads.
  /// @tparam V Iterable type. Pre-conditions: elements must be addable to each other and must be MPI reducible
  /// @param data Container with data
  /// @return Mean of data on all threads reduced to all threads; type is deduced from first element of :param:`data`
  /// @example triqs/stat/mean_2.cpp
  /// @brief Calculate mean (MPI Version)
  template <typename V> auto mean_mpi(mpi::communicator c, V const &data) {
    // ENSURE(data.size() > 0);
    auto M = make_regular(mean(data));
    long N = mpi::all_reduce(data.size(), c);
    M *= double(data.size()) / N;
    mpi::all_reduce_in_place(M, c); //M = mpi::all_reduce(M, c, 0);
    return M;
  }

  /// Calculate arithmetic mean and standard error [REF] of data in an iterable container.
  /// @tparam V Iterable type. Pre-conditions: elements must be: addable to each other, have element-wise multiplication, have complex conjugation defined via conj_r [REF] .
  /// @param data Container with data
  /// @return std::pair with [mean, standard error]; types are deduced from first element of :param:`data`
  /// @example triqs/stat/mean_err_1.cpp
  /// @brief Calculate mean and standard error
  template <typename V> auto mean_and_err(V const &data) {
    // ENSURE(data.size() > 0);
    using std::real;
    using std::sqrt;
    using triqs::arrays::conj_r;
    long length    = data.size();
    auto mean_calc = mean(data);
    auto err_calc  = make_regular(real(mean_calc));
    err_calc       = 0;
    for (auto const &x : data) { err_calc += conj_r(x - mean_calc) * (x - mean_calc) / (length * (length - 1)); }
    err_calc = sqrt(err_calc);
    return std::make_pair(mean_calc, err_calc);
  }

  /// Calculate arithmetic mean and standard error [REF] of data spread over multiple MPI threads. Reduces the answer to all threads.
  /// @tparam V Iterable type. Pre-conditions: elements must be: addable to each other, have element-wise multiplication, have complex conjugation defined via conj_r [REF], be MPI reducable.
  /// @param data Container with data
  /// @return std::pair with [mean, standard error]; types are deduced from first element of :param:`data`
  /// @example triqs/stat/mean_err_2.cpp
  /// @brief Calculate mean and standard error (MPI Version)
  template <typename V> auto mean_and_err_mpi(mpi::communicator c, V const &data) {
    // ENSURE(data.size() > 0);
    using std::real;
    using std::sqrt;
    using triqs::arrays::conj_r;
    long length    = mpi::all_reduce(data.size(), c);
    auto mean_calc = mean_mpi(c, data);
    auto err_calc  = make_regular(real(mean_calc));
    err_calc       = 0;
    for (auto const &x : data) { err_calc += conj_r(x - mean_calc) * (x - mean_calc) / (length * (length - 1)); }
    mpi::all_reduce_in_place(err_calc, c);
    err_calc = sqrt(err_calc);
    return std::make_pair(mean_calc, err_calc);
  }
} // namespace triqs::stat
