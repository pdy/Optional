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

//#include "Common.hpp"

#include <type_traits>

TEST(TraitsUT, removeConst)
{
  using T = int;
  using Const_T = const int;
  
 // const bool stdWorks = std::is_same_v<T, std::remove_const_t<Const_T> >;
//  ASSERT_TRUE(stdWorks);

  const bool libWorks = std::is_same_v<T, detail::non_const_t<Const_T>>;
  EXPECT_TRUE(libWorks);
}

TEST(TraitsUT, noexceptMoveCtor)
{
  struct TypeNoexceptMove
  {
    int placeholder {0};

    TypeNoexceptMove() = default;
    TypeNoexceptMove(TypeNoexceptMove&&) = default;
  };

  struct TypeThrowMove
  {
    int placeholder {0};

    TypeThrowMove() = default;
    TypeThrowMove(TypeThrowMove&&) {}
  };

  EXPECT_TRUE(detail::is_noxcept_move_constructble<int>::value);
  EXPECT_TRUE(detail::is_noxcept_move_constructble<TypeNoexceptMove>::value);
  EXPECT_FALSE(detail::is_noxcept_move_constructble<TypeThrowMove>::value);
}
