// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <units/concepts.h>
#include <units/dimension.h>
#include <units/unit.h>

namespace units {

/**
 * @brief The type for quantity references
 *
 * This type is intended to be used in the quantity references definition:
 *
 * @code{.cpp}
 * namespace length_references {
 *
 * inline constexpr auto m = reference<dim_length, metre>{};
 * inline constexpr auto km = reference<dim_length, kilometre>{};
 *
 * }
 *
 * namespace references {
 *
 * using namespace length_references;
 *
 * }
 * @endcode
 *
 * Quantity references simplify quantity creation:
 *
 * @code{.cpp}
 * using namespace units::isq::si::references;
 *
 * auto d = 123 * m;
 * auto v = 70 * (km / h);
 * @endcode
 *
 * Also, it is allowed to define custom quantity references from existing ones:
 *
 * @code{.cpp}
 * constexpr auto Nm = N * m;
 * constexpr auto mph = mi / h;
 * @endcode
 *
 * The following syntaxes are not allowed:
 * `2 / s`, `km * 3`, `s / 4`, `70 * km / h`.
 */
template<Dimension auto D, Unit auto U>
struct reference {
  static constexpr auto dimension = D;
  static constexpr auto unit = U;
};

// Reference

template<Magnitude M, Reference R>
[[nodiscard]] consteval reference<R::dimension, M{} * R::unit> operator*(M, R)
{
  return {};
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval reference<R1::dimension * R2::dimension, R1::unit * R2::unit> operator*(R1, R2)
{
  return {};
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval reference<R1::dimension / R2::dimension, R1::unit / R2::unit> operator/(R1, R2)
{
  return {};
}

template<Representation Rep, Reference R>
[[nodiscard]] constexpr quantity<R{}, Rep> operator*(const Rep& lhs, R)
{
  return quantity<R{}, Rep>(lhs);
}

void /*Use `q * (1 * r)` rather than `q * r`.*/ operator*(Quantity auto, Reference auto) = delete;

template<Reference R1, Reference R2>
[[nodiscard]] consteval bool operator==(R1, R2)
{
  return R1::dimension == R2::dimension && R1::unit == R2::unit;
}

template<Reference R1, Reference R2>
[[nodiscard]] consteval bool interconvertible(R1, R2)
{
  return interconvertible(R1::dimension, R2::dimension) && interconvertible(R1::unit, R2::unit);
}

[[nodiscard]] consteval auto common_reference(Reference auto r1, Reference auto r2, Reference auto... rest)
  requires requires {
    {
      common_dimension(r1.dimension, r2.dimension, rest.dimension...)
    } -> Dimension;
    {
      common_unit(r1.unit, r2.unit, rest.unit...)
    } -> Unit;
  }
{
  return reference<common_dimension(r1.dimension, r2.dimension, rest.dimension...),
                   common_unit(r1.unit, r2.unit, rest.unit...)>{};
}

}  // namespace units
