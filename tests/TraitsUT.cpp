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

TEST(TraitsUT, removeConst)
{
  using T = int;
  using Const_T = const int;

  const bool expectEq = std::is_same_v<T, detail::non_const_t<Const_T>>;
  EXPECT_TRUE(expectEq);
}

TEST(TraitsUT, noexceptMoveCtor)
{
  struct TypeDefaultMove
  {
    int placeholder {0};

    TypeDefaultMove() = default;
    TypeDefaultMove(TypeDefaultMove&&) = default;
  };

  struct TypeNoexceptMove
  {
    int placeholder {0};

    TypeNoexceptMove() = default; 
    TypeNoexceptMove(TypeNoexceptMove&&) noexcept {}
  };
  
  struct TypeNoexceptIntMove
  {
    int placeholder {0};

    TypeNoexceptIntMove() = default; 
    TypeNoexceptIntMove(TypeNoexceptIntMove&&) noexcept(noexcept(std::is_move_constructible_v<decltype(placeholder)>)) {}
  };
  
  struct TypeThrowMove
  {
    int placeholder {0};

    TypeThrowMove() = default;
    TypeThrowMove(TypeThrowMove&&) {}
  };

  struct TypeExplicitThrowMove
  {
    int placeholder {0};

    TypeExplicitThrowMove() = default;
    TypeExplicitThrowMove(const TypeExplicitThrowMove&) noexcept(false) {}
  };

  EXPECT_TRUE(detail::is_noxcept_move_constructible<int>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructible<TypeDefaultMove>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructible<TypeNoexceptMove>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructible<TypeNoexceptIntMove>::value);

  EXPECT_FALSE(detail::is_noxcept_move_constructible<TypeThrowMove>::value);
  EXPECT_FALSE(detail::is_noxcept_move_constructible<TypeExplicitThrowMove>::value);
}

TEST(TraitsUT, noexceptCopyCtor)
{
  struct TypeDefaultCopy
  {
    int placeholder {0};

    TypeDefaultCopy() = default;
    TypeDefaultCopy(const TypeDefaultCopy&) = default;
  };

  struct TypeNoexceptCopy
  {
    int placeholder {0};

    TypeNoexceptCopy() = default; 
    TypeNoexceptCopy(const TypeNoexceptCopy&) noexcept {}
  };

  struct TypeNoexceptIntCopy
  {
    int placeholder {0};

    TypeNoexceptIntCopy() = default; 
    TypeNoexceptIntCopy(const TypeNoexceptIntCopy&) noexcept(noexcept(std::is_nothrow_copy_constructible_v<decltype(placeholder)>)) {}
  };

  struct TypeThrowCopy
  {
    int placeholder {0};

    TypeThrowCopy() = default;
    TypeThrowCopy(const TypeThrowCopy&) {}
  };

  struct TypeExplicitThrowCopy
  {
    int placeholder {0};

    TypeExplicitThrowCopy() = default;
    TypeExplicitThrowCopy(const TypeExplicitThrowCopy&) noexcept(false) {}
  };

  EXPECT_TRUE(detail::is_noxcept_move_constructible<int>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructible<TypeDefaultCopy>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructible<TypeNoexceptCopy>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructible<TypeNoexceptIntCopy>::value);

  EXPECT_FALSE(detail::is_noxcept_move_constructible<TypeThrowCopy>::value);
  EXPECT_FALSE(detail::is_noxcept_move_constructible<TypeExplicitThrowCopy>::value);
}

TEST(TraitsUT, conditional)
{
  using OnTrue = typename detail::conditional<true, int, double>::type;
  using OnFalse = typename detail::conditional<false, int, double>::type;

  const bool expectInt = std::is_same_v<int, OnTrue>;
  const bool expectDouble = std::is_same_v<double, OnFalse>;

  EXPECT_TRUE(expectInt);
  EXPECT_TRUE(expectDouble);
}

template<typename T>
class TraitsIsArithmetic : public testing::Test
{};

TYPED_TEST_SUITE_P(TraitsIsArithmetic);

// positive test cases places in Optional_20_UT as it already consist types list
TYPED_TEST_P(TraitsIsArithmetic, isArithmeticNegative)
{
  const bool isArithmetic = std::is_same_v<std::true_type, detail::is_arithmetic_t<TypeParam>>;
  EXPECT_FALSE(isArithmetic);
}

REGISTER_TYPED_TEST_SUITE_P(TraitsIsArithmetic, isArithmeticNegative);

using TestTypes = ::testing::Types<std::string, std::vector<int>, Optional<int>>;
 
INSTANTIATE_TYPED_TEST_SUITE_P(My, TraitsIsArithmetic, TestTypes);

