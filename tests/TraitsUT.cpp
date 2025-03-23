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

namespace {
  struct HasNoexceptSwap
  {
    int placeholder;

    friend void swap(HasNoexceptSwap &lhs, HasNoexceptSwap &rhs) noexcept
    {
      using std::swap;
      swap(lhs.placeholder, rhs.placeholder);
    }
  };

  struct HasNoNoexceptSwap
  {
    int placeholder;

    friend void swap(HasNoNoexceptSwap &lhs, HasNoNoexceptSwap &rhs)
    {
      using std::swap;
      swap(lhs.placeholder, rhs.placeholder);
    }
  };
}

TEST(TraitsUT, noexcepetSwappable)
{
  EXPECT_TRUE(detail::is_noexcept_swappable<HasNoexceptSwap>::value);
  EXPECT_FALSE(detail::is_noexcept_swappable<HasNoNoexceptSwap>::value);
}

TEST(TraitsUT, conditional)
{
  using OnTrue = typename detail::conditional_type<true, int, double>::type;
  using OnFalse = typename detail::conditional_type<false, int, double>::type;

  const bool expectInt = std::is_same_v<int, OnTrue>;
  const bool expectDouble = std::is_same_v<double, OnFalse>;

  EXPECT_TRUE(expectInt);
  EXPECT_TRUE(expectDouble);
}

TEST(TraitsUt, addRvalueRef)
{
  {
    using T = int;
    using Rval_T = int&&;
    using ConstRval_T = const int&&;
    using Lref_T = int&;
    using PtrRval_T = int*&&;
    using FuncDeclRval_T = int(&&)(int);

    using Actual = typename detail::add_rvalue_ref<T>::type;
    using ActualPtr = typename detail::add_rvalue_ref<T*>::type;
    using ActualConst = typename detail::add_rvalue_ref<const T>::type;
    using ActualOnRval = typename detail::add_rvalue_ref<Rval_T>::type;
    using ActualOnLref_T = typename detail::add_rvalue_ref<Lref_T>::type;
    using ActualOnFunc = typename detail::add_rvalue_ref<T(T)>::type;

    const bool sameTypes = std::is_same_v<Rval_T, Actual>;
    const bool sameOnPtr = std::is_same_v<PtrRval_T, ActualPtr>;
    const bool sameOnConst = std::is_same_v<ConstRval_T, ActualConst>;
    const bool sameTypes_OnRval = std::is_same_v<Rval_T, ActualOnRval>;
    const bool sameTypes_OnLref_T = std::is_same_v<Lref_T, ActualOnLref_T>;
    const bool sameOnFunc = std::is_same_v<FuncDeclRval_T, ActualOnFunc>;

    EXPECT_TRUE(sameTypes);
    EXPECT_TRUE(sameOnPtr);
    EXPECT_TRUE(sameOnConst);
    EXPECT_TRUE(sameTypes_OnRval);
    EXPECT_TRUE(sameTypes_OnLref_T);
    EXPECT_TRUE(sameOnFunc);
  }

  {
    class ClassType {};
    using T = ClassType;
    using Rval_T = ClassType&&;
    using ConstRval_T = const ClassType&&;
    using Lref_T = ClassType&;
    using PtrRval_T = ClassType*&&;
    using FuncDeclRval_T = ClassType(&&)(ClassType);

    using Actual = typename detail::add_rvalue_ref<T>::type;
    using ActualPtr = typename detail::add_rvalue_ref<T*>::type;
    using ActualConst = typename detail::add_rvalue_ref<const T>::type;
    using ActualOnRval = typename detail::add_rvalue_ref<Rval_T>::type;
    using ActualOnLref_T = typename detail::add_rvalue_ref<Lref_T>::type;
    using ActualOnFunc = typename detail::add_rvalue_ref<T(T)>::type;

    const bool sameTypes = std::is_same_v<Rval_T, Actual>;
    const bool sameOnPtr = std::is_same_v<PtrRval_T, ActualPtr>;
    const bool sameOnConst = std::is_same_v<ConstRval_T, ActualConst>;
    const bool sameTypes_OnRval = std::is_same_v<Rval_T, ActualOnRval>;
    const bool sameTypes_OnLref_T = std::is_same_v<Lref_T, ActualOnLref_T>;
    const bool sameOnFunc = std::is_same_v<FuncDeclRval_T, ActualOnFunc>;

    EXPECT_TRUE(sameTypes);
    EXPECT_TRUE(sameOnPtr);
    EXPECT_TRUE(sameOnConst);
    EXPECT_TRUE(sameTypes_OnRval);
    EXPECT_TRUE(sameTypes_OnLref_T);
    EXPECT_TRUE(sameOnFunc);
  }
}

TEST(TraitsUT, isTriviallyDestructible)
{
  class EmptyClassType {};
  class ClassType
  {
    ~ClassType() = default;
  };

  class ClassType_2
  {
    ~ClassType_2() {}
  };

  EXPECT_TRUE(detail::is_trivially_destructible<int>::value);
  EXPECT_TRUE(detail::is_trivially_destructible<double>::value);
  EXPECT_TRUE(detail::is_trivially_destructible<EmptyClassType>::value);

  EXPECT_FALSE(detail::is_trivially_destructible<ClassType>::value);
  EXPECT_FALSE(detail::is_trivially_destructible<ClassType_2>::value);

  EXPECT_TRUE(detail::is_trivially_destructible<const int>::value);
  EXPECT_TRUE(detail::is_trivially_destructible<const double>::value);
  EXPECT_TRUE(detail::is_trivially_destructible<const EmptyClassType>::value);

  EXPECT_FALSE(detail::is_trivially_destructible<const ClassType>::value);
  EXPECT_FALSE(detail::is_trivially_destructible<const ClassType_2>::value);
}

TEST(TraitsUT, isNoexceptDestructible)
{
  class EmptyClassType {};
  class ClassType
  {
    ~ClassType() = default;
  };
  
  class ClassType_1
  {
    ~ClassType_1() noexcept {}
  };

  class ClassType_2
  {
    ~ClassType_2() {}
  };

  EXPECT_TRUE(detail::is_noexcept_destructible<int>::value);
  EXPECT_TRUE(detail::is_noexcept_destructible<double>::value);
  EXPECT_TRUE(detail::is_noexcept_destructible<EmptyClassType>::value);
  EXPECT_FALSE(detail::is_noexcept_destructible<ClassType>::value);
  EXPECT_FALSE(detail::is_noexcept_destructible<ClassType_1>::value);

  EXPECT_FALSE(detail::is_noexcept_destructible<ClassType_2>::value);

  EXPECT_TRUE(detail::is_noexcept_destructible<const int>::value);
  EXPECT_TRUE(detail::is_noexcept_destructible<const double>::value);
  EXPECT_TRUE(detail::is_noexcept_destructible<const EmptyClassType>::value);
  EXPECT_FALSE(detail::is_noexcept_destructible<const ClassType>::value);
  EXPECT_FALSE(detail::is_noexcept_destructible<const ClassType_1>::value);

  EXPECT_FALSE(detail::is_noexcept_destructible<const ClassType_2>::value);
}


template<typename T>
class TraitsIsArithmetic : public testing::Test
{};

TYPED_TEST_SUITE_P(TraitsIsArithmetic);

// positive test cases places in Optional_20_UT as it already consist types list
TYPED_TEST_P(TraitsIsArithmetic, isArithmeticNegative)
{
  EXPECT_FALSE(detail::is_arithmetic<TypeParam>::value);
}

REGISTER_TYPED_TEST_SUITE_P(TraitsIsArithmetic, isArithmeticNegative);

using TestTypes = ::testing::Types<std::string, std::vector<int>, Optional<int>>;
 
INSTANTIATE_TYPED_TEST_SUITE_P(My, TraitsIsArithmetic, TestTypes);

