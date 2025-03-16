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


namespace {

template<typename T>
void check_arith_empty()
{
  const Optional<T> empty;

  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_EQ(T{5}, empty.value_or(T{5}));
  EXPECT_TRUE(util::size_check<T>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(empty)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(empty)>::value);
}

template<typename TypeParam>
void check_arith_ctor_and_reset()
{
  Optional<TypeParam> val(TypeParam{10});

  EXPECT_TRUE(val);
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val.has_value());
  EXPECT_EQ(TypeParam{10}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(util::size_check<TypeParam>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

  val.reset();

  EXPECT_FALSE(val);
  EXPECT_TRUE(!val);
  EXPECT_FALSE(val.has_value());
  EXPECT_EQ(TypeParam{5}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(util::size_check<TypeParam>());
  EXPECT_TRUE(util::has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

}

} // namespace

TEST(Optional_11_UT, empty)
{
  check_arith_empty<int>(); 
  check_arith_empty<unsigned>(); 
  check_arith_empty<char>(); 
  check_arith_empty<uint8_t>(); 
  check_arith_empty<size_t>(); 
  check_arith_empty<double>(); 
}

TEST(Optional_11_UT, ctorAndReset)
{
  check_arith_ctor_and_reset<int>(); 
  check_arith_ctor_and_reset<unsigned>(); 
  check_arith_ctor_and_reset<char>(); 
  check_arith_ctor_and_reset<uint8_t>(); 
  check_arith_ctor_and_reset<size_t>(); 
  check_arith_ctor_and_reset<double>();
}

TEST(Optional_11_UT, observeEmptyCtor)
{
  const Optional<util::Observe> empty;
  
  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_TRUE(util::size_check<util::Observe>());
  EXPECT_FALSE(util::has_noexcept_swap<decltype(empty)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(empty)>::value);
}

TEST(Optional_11_UT, observeMoveCtor)
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

TEST(Optional_11_UT, observeMoveCtorWithCallable)
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

TEST(Optional_11_UT, dtorCalledOnReset)
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
