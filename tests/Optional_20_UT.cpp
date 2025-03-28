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

#include "Common.hpp"

#include <type_traits>
#include <cstddef>
#include <cstdint>

template<typename T>
class Optional_20_ArithTests : public testing::Test
{};

TYPED_TEST_SUITE_P(Optional_20_ArithTests);

TYPED_TEST_P(Optional_20_ArithTests, isArithmetic)
{
  // this really should be in TraitsUT but I don't see the reason to create such typed list repetition
  EXPECT_TRUE(detail::is_arithmetic<TypeParam>::value);
}

TYPED_TEST_P(Optional_20_ArithTests, emptyTest)
{
  const Optional<TypeParam> empty;

  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_EQ(TypeParam{5}, empty.value_or(TypeParam{5}));
  EXPECT_TRUE(util::size_check<TypeParam>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(empty)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(empty)>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(empty)>);
}

TYPED_TEST_P(Optional_20_ArithTests, ctorAndReset)
{
  Optional<TypeParam> val(TypeParam{10});

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(util::size_check<TypeParam>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val)>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(val)>);

  val.reset();

  EXPECT_FALSE(val);
  EXPECT_TRUE(!val);
  EXPECT_FALSE(val.has_value());
  EXPECT_EQ(TypeParam{5}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(util::size_check<TypeParam>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val)>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(val)>);
}

TYPED_TEST_P(Optional_20_ArithTests, assignValue)
{
  const Optional<TypeParam> val = TypeParam{10};

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
}

TYPED_TEST_P(Optional_20_ArithTests, assingToEmpty)
{
  Optional<TypeParam> val;
  val = TypeParam{10};

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
}

TYPED_TEST_P(Optional_20_ArithTests, returnFromCallable)
{
  const auto callable = []() -> Optional<TypeParam> { return TypeParam{10}; };
  const auto callable_2 = []() -> Optional<TypeParam> { return Optional<TypeParam>{TypeParam{10}}; };

  const auto val = callable();
  const auto val_2 = callable_2();

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(util::size_check<TypeParam>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val)>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(val)>);

  EXPECT_TRUE(val_2);
  EXPECT_FALSE(!val_2);
  EXPECT_TRUE(val_2.has_value());
  EXPECT_EQ(TypeParam{10}, val_2.value_or(TypeParam{5}));
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val_2)>());
  EXPECT_TRUE(std::is_trivially_destructible_v<decltype(val_2)>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(val_2)>);
}

REGISTER_TYPED_TEST_SUITE_P(
    Optional_20_ArithTests,
    emptyTest,
    ctorAndReset,
    assignValue,
    assingToEmpty,
    returnFromCallable,
    isArithmetic
);

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
 
INSTANTIATE_TYPED_TEST_SUITE_P(My, Optional_20_ArithTests, TestTypes);


TEST(Optional_20_UT, observeEmptyCtor)
{
  const Optional<util::Observe> empty;
  
  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_TRUE(util::size_check<util::Observe>());
  EXPECT_FALSE(util::has_noexcept_swap<decltype(empty)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(empty)>::value);
}

TEST(Optional_20_UT, observeMoveCtor)
{
  const Optional<util::Observe> val{util::Observe{}};
  
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val);
  EXPECT_TRUE(val.has_value());
  EXPECT_TRUE(util::size_check<util::Observe>());
  EXPECT_FALSE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

  EXPECT_EQ(util::Event::MoveCtor, val->event);
}

TEST(Optional_20_UT, observeMoveCtorWithCallable)
{
  const auto callable = []() -> Optional<util::Observe>
  {
    util::Observe ret;
    return ret;
  };

  const auto val = callable();
  
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val);
  EXPECT_TRUE(val.has_value());
  EXPECT_TRUE(util::size_check<util::Observe>());
  EXPECT_FALSE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

  EXPECT_EQ(util::Event::MoveCtor, val->event);
}

TEST(Optional_20_UT, dtorCalledOnReset)
{
  unsigned dtorCalled = 0;
  
  Optional<util::DtorCalled> val{util::DtorCalled(dtorCalled)};

  EXPECT_FALSE(!val);
  EXPECT_TRUE(val);
  EXPECT_TRUE(val.has_value());
  EXPECT_TRUE(util::size_check<util::DtorCalled>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_FALSE(std::is_trivially_destructible<decltype(val)>::value);

  EXPECT_EQ(1, dtorCalled); // moved from object called dtor

  val.reset(); // we should call dtor here as well

  EXPECT_EQ(2, dtorCalled);

}
