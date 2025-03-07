/*
* MIT License
*
* Copyright (c) 2025 Pawel Drzycimski
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <gtest/gtest.h>

#include <Optional.hpp>
#include <type_traits>

#include <cstddef>
#include <cstdint>

namespace {

template<typename T>
constexpr bool size_check()
{
  return sizeof(Optional<T>) == sizeof(T) + std::alignment_of<T>::value;
}

template<typename T>
constexpr bool has_noexcept_swap()
{
  using NonConst_T = std::remove_const<T>;
  return noexcept(swap(std::declval<NonConst_T&>(), std::declval<NonConst_T&>()));
}

} // namespace

template<typename T>
class OptionalArithmeticTests : public testing::Test
{};

TYPED_TEST_SUITE_P(OptionalArithmeticTests);

TYPED_TEST_P(OptionalArithmeticTests, emptyTest)
{
  const Optional<TypeParam> empty;

  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_EQ(TypeParam{5}, empty.value_or(TypeParam{5}));
  EXPECT_TRUE(size_check<TypeParam>());
  EXPECT_TRUE(has_noexcept_swap<decltype(empty)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(empty)>);
}

TYPED_TEST_P(OptionalArithmeticTests, ctorAndReset)
{
  Optional<TypeParam> val(TypeParam{10});

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(size_check<TypeParam>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val)>);

  val.reset();

  EXPECT_FALSE(val);
  EXPECT_TRUE(!val);
  EXPECT_FALSE(val.has_value());
  EXPECT_EQ(TypeParam{5}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(size_check<TypeParam>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val)>);
}

TYPED_TEST_P(OptionalArithmeticTests, assignValue)
{
  const Optional<TypeParam> val = TypeParam{10};

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
}

TYPED_TEST_P(OptionalArithmeticTests, assingToEmpty)
{
  Optional<TypeParam> val;
  val = TypeParam{10};

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
}

TYPED_TEST_P(OptionalArithmeticTests, returnFromCallable)
{
  const auto callable = []() -> Optional<TypeParam> { return TypeParam{10}; };
  const auto callable_2 = []() -> Optional<TypeParam> { return Optional<TypeParam>{TypeParam{10}}; };

  const auto val = callable();
  const auto val_2 = callable_2();

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(size_check<TypeParam>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val)>);

  EXPECT_TRUE(val_2);
  EXPECT_FALSE(!val_2);
  EXPECT_TRUE(val_2.has_value());
  EXPECT_EQ(TypeParam{10}, val_2.value_or(TypeParam{5}));
  EXPECT_TRUE(has_noexcept_swap<decltype(val_2)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val_2)>);
}

REGISTER_TYPED_TEST_SUITE_P(OptionalArithmeticTests, emptyTest, ctorAndReset, assignValue, assingToEmpty, returnFromCallable);

using TestTypes = ::testing::Types<
  char, signed char, unsigned char,
  short, short int, signed short, signed short int, unsigned short, unsigned short int,
  int, signed, signed int, unsigned, unsigned int,
  long, long int, signed long, signed long int, long long, long long int, signed long long, signed long long int, unsigned long long, unsigned long long int,
  float, double, long double,
  // cstddef
  size_t, ptrdiff_t, 
  // cstdint
  int8_t, int16_t, int32_t, int64_t, int_fast8_t, int_fast16_t, int_fast32_t, int_fast64_t, int_least8_t, int_least16_t, int_least32_t, int_least64_t,
  uint8_t, uint16_t, uint32_t, uint64_t, uint_fast8_t, uint_fast16_t, uint_fast32_t, uint_fast64_t, uint_least8_t, uint_least16_t, uint_least32_t, uint_least64_t,
  intmax_t, intptr_t, uintmax_t, uintptr_t
>;
 
INSTANTIATE_TYPED_TEST_SUITE_P(My, OptionalArithmeticTests, TestTypes);
