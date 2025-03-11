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

template<typename T>
void check_arith_empty()
{
  const Optional<T> empty;

  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_EQ(T{5}, empty.value_or(T{5}));
  EXPECT_TRUE(size_check<T>());
  EXPECT_TRUE(has_noexcept_swap<decltype(empty)>());
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
  EXPECT_TRUE(size_check<TypeParam>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

  val.reset();

  EXPECT_FALSE(val);
  EXPECT_TRUE(!val);
  EXPECT_FALSE(val.has_value());
  EXPECT_EQ(TypeParam{5}, val.value_or(TypeParam{5}));
  EXPECT_TRUE(size_check<TypeParam>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

}

enum class Event
{
  DefaultCtor,
  CopyCtor,
  MoveCtor
};

struct Observe
{
  Event event;
  int placeholder;

  Observe()
    : event{Event::DefaultCtor}, placeholder{0}
  {}

  Observe(const Observe&)
    : event{Event::CopyCtor}, placeholder{0}
  {}

  Observe(Observe&&)
    : event{Event::MoveCtor}, placeholder{0}
  {}

};

struct DtorCalled
{
  bool &dtorCalled;

  DtorCalled(const DtorCalled&) = default;
  DtorCalled(DtorCalled&&) = default;

  DtorCalled(bool &dtorCalledRef)
    : dtorCalled{dtorCalledRef}
  {
  }

  ~DtorCalled()
  {
    dtorCalled = true;
  }
};

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
  const Optional<Observe> empty;
  
  EXPECT_FALSE(empty);
  EXPECT_TRUE(!empty);
  EXPECT_FALSE(empty.has_value());
  EXPECT_TRUE(size_check<Observe>());
  EXPECT_TRUE(has_noexcept_swap<decltype(empty)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(empty)>::value);
}

TEST(Optional_11_UT, observeMoveCtor)
{
  const Optional<Observe> val{Observe{}};
  
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val);
  EXPECT_TRUE(val.has_value());
  EXPECT_TRUE(size_check<Observe>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

  EXPECT_EQ(Event::MoveCtor, val->event);
}

TEST(Optional_11_UT, observeMoveCtorWithCallable)
{
  const auto callable = []() -> Optional<Observe>
  {
    Observe ret;
    return ret;
  };

  const auto val = callable();
  
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val);
  EXPECT_TRUE(val.has_value());
  EXPECT_TRUE(size_check<Observe>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_TRUE(std::is_trivially_destructible<decltype(val)>::value);

  EXPECT_EQ(Event::MoveCtor, val->event);
}

TEST(Optional_11_UT, dtorCalledOnReset)
{
  bool dtorCalled = false;
  
  Optional<DtorCalled> val = DtorCalled(dtorCalled);
  
  EXPECT_FALSE(!val);
  EXPECT_TRUE(val);
  EXPECT_TRUE(val.has_value());
  EXPECT_TRUE(size_check<DtorCalled>());
  EXPECT_TRUE(has_noexcept_swap<decltype(val)>());
  EXPECT_FALSE(std::is_trivially_destructible<decltype(val)>::value);

  EXPECT_TRUE(dtorCalled);
}
