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


#include <Optional.hpp>
#include <type_traits>

namespace util {

template<typename T>
constexpr bool size_check()
{
  return sizeof(Optional<T>) == sizeof(T) + std::alignment_of<T>::value;
}

template<typename T>
constexpr bool has_noexcept_swap()
{
  using NonConst_T = typename std::remove_const<T>::type;
  return noexcept(swap(std::declval<NonConst_T&>(), std::declval<NonConst_T&>()));
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
  unsigned &dtorCalled;

  DtorCalled(const DtorCalled&) = default;
  DtorCalled(DtorCalled&&) = default;

  DtorCalled(unsigned &dtorCalledRef)
    : dtorCalled{dtorCalledRef}
  {
  }

  ~DtorCalled()
  {
    ++dtorCalled;
  }
};

} // namespace util
