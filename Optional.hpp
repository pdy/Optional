/*
*  MIT License
*  
*  Copyright (c) 2025 Pawel Drzycimski
*  
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*  
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*  
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*
*/

#ifndef PDY_OPTIONAL_HPP_
#define PDY_OPTIONAL_HPP_

#include <memory>
#include <type_traits>
#include <cassert>

namespace detail {

template<typename T>
struct remove_const
{
  using type = T;
};

template<typename T>
struct remove_const<const T>
{
  using type = T;
};

template<typename T>
using non_const_t = typename remove_const<T>::type;

#if 0
template<bool B, typename T = void>
struct enable_if
{
  using type = T;
};

template<typename T>
struct enable_if<false, T>
{};
#endif

template<typename T>
struct add_rvalue_ref
{
  using type = typename std::add_rvalue_reference<T>::type;
};

template<typename T>
typename add_rvalue_ref<T>::type declval() noexcept
{
  static_assert(false, "declval not allowed in an evaluated context");
}

template<typename T>
struct is_noexcept_swappable
{
  using NonConst_T = non_const_t<T>;
  static constexpr bool value = noexcept(swap(detail::declval<NonConst_T&>(), detail::declval<NonConst_T&>()));
};

template<typename T>
struct is_noxcept_move_constructible 
{
  using NonConst_T = non_const_t<T>;
  static constexpr bool value = noexcept(NonConst_T(declval<NonConst_T&&>())); 
};

template<typename T>
struct is_noexcept_copy_constructible
{
  using NonConst_T = non_const_t<T>;
  static constexpr bool value = noexcept(NonConst_T(declval<NonConst_T&>())); 
};

template<typename T>
struct is_trivially_destructible : std::is_trivially_destructible<T> {};

template<typename T>
struct is_noexcept_destructible : std::is_nothrow_destructible<T> {};

#if 0
template<bool isTrivialDtor, typename T>
struct is_noexcept_destructible_helper {};

template<typename T>
struct is_noexcept_destructible_helper<true, T>
{
  static constexpr bool value = true;
};

template<typename T>
struct is_noexcept_destructible_helper<false, T>
{
  using NonConst_T = non_const_t<T>;
  static constexpr bool value = noexcept(declval<NonConst_T&>().~NonConst_T());
};

template<typename T>
struct is_noexcept_destructible
  : is_noexcept_destructible_helper<is_trivially_destructible<T>::value, T>
{};
#endif

template<typename T>
struct is_arithmetic : std::is_arithmetic<T> {};

template<bool SWITCH, typename T, typename U>
struct conditional_type {};

template<typename T, typename U>
struct conditional_type<true, T, U>
{
  using type = T;
};

template<typename T, typename U>
struct conditional_type<false, T, U>
{
  using type = U;
};

template<bool isArithmetic, typename T, typename TSelf>
struct AddArrowOperator {};

template<typename T, typename TSelf>
struct AddArrowOperator<false, T, TSelf>
{
  const T* operator->() const { return &(static_cast<const TSelf*>(this)->value()); }
  T* operator->() { return &(static_cast<TSelf*>(this)->value()); }
};

template<typename T>
struct storage_trivial_dtor
{
  union {
    char dummy;
    T value;
  };

  bool engaged;

  explicit constexpr storage_trivial_dtor() noexcept
    : dummy{0}, engaged{false}
  {}

  explicit constexpr storage_trivial_dtor(const T &val) noexcept
    : value{val}, engaged{true}
  {}

  explicit constexpr storage_trivial_dtor(T &&val) noexcept
    : value{std::move(val)}, engaged{true}
  {}

  ~storage_trivial_dtor() = default;
};

template<typename T>
struct storage_non_trivial_dtor
{
  union {
    char dummy;
    T value;
  };

  bool engaged;

  explicit constexpr storage_non_trivial_dtor() noexcept
    : dummy{0}, engaged{false}
  {}

  explicit constexpr storage_non_trivial_dtor(const T &val) noexcept
    : value{val}, engaged{true}
  {}

  explicit constexpr storage_non_trivial_dtor(T &&val) noexcept
    : value{std::move(val)}, engaged{true}
  {}
  
  ~storage_non_trivial_dtor() noexcept(is_noexcept_destructible<T>::value)
  {
    if(engaged)
      value.T::~T();
  }
};

template<typename T>
using optional_storage = typename conditional_type<
    is_trivially_destructible<non_const_t<T>>::value,
    storage_trivial_dtor<non_const_t<T>>,
    storage_non_trivial_dtor<non_const_t<T>>>::type;

} // namespace detail

template<typename T>
class Optional final : public detail::AddArrowOperator<detail::is_arithmetic<T>::value, T, Optional<T>>
{
  detail::optional_storage<T> m_storage;

  constexpr const T* get() const { return std::addressof(m_storage.value); }
  detail::non_const_t<T>* get() { return std::addressof(m_storage.value); }

  template<typename ...Args>
  void construct(Args&& ...args)
  {
    assert(!m_storage.engaged);

    // TODO: this will not work for an array
    // is it a real concern?
    ::new(static_cast<void*>(get())) T(std::forward<Args>(args)...);
    m_storage.engaged = true;
  }

public:
  Optional() = default; 

  constexpr Optional(const T &val) noexcept(detail::is_noexcept_copy_constructible<T>::value)
    : m_storage(val)
  {}

  constexpr Optional(T &&val) noexcept(detail::is_noxcept_move_constructible<T>::value)
    : m_storage(std::move(val))
  {}

  Optional(const Optional<T> &other) noexcept(detail::is_noexcept_copy_constructible<T>::value)
    : m_storage()
  {
    if(other.has_value())
      construct(*other);
  }

  Optional(Optional<T> &&other) noexcept(detail::is_noxcept_move_constructible<T>::value)
    : m_storage()
  {
    if(other.has_value())
      construct(*other);
  }

  ~Optional() = default;

  const T& operator*() const & { assert(has_value()); return m_storage.value; }
  T& operator*() & { assert(has_value()); return m_storage.value; }

  T&& operator*() && { assert(has_value()); return std::move(m_storage.value); }

  constexpr explicit operator bool() const noexcept { return m_storage.engaged; }
  constexpr bool has_value() const noexcept { return m_storage.engaged; }

  const T& value() const & { return **this; }
  T& value() & { return **this; }

  T&& value() && { return std::move(**this); }

  template<typename U = detail::non_const_t<T>>
  T value_or(U &&u) const&
  {
    if(has_value())
      return **this;

    return std::forward<U>(u);
  }

  template<typename U = detail::non_const_t<T>>
  T&& value_or(U &&u) &&
  {
    if(has_value())
      return std::move(**this);

    return std::forward<U>(u);
  }

  void reset() noexcept(detail::is_noexcept_destructible<T>::value)
  {
    if(has_value())
      get()->T::~T();

    m_storage.engaged = false;
  }

  template<typename U = T>
  Optional<T>& operator=(U &&val)
  {
    if(has_value())
      m_storage.value = std::forward<U>(val);
    else
      construct(std::forward<U>(val));

    return *this;
  }

  Optional<T>& operator=(Optional<T> other) noexcept(detail::is_noexcept_swappable<Optional<T>>::value)
  {
    swap(*this, other);
    return *this;
  }

  friend void swap(Optional<T> &lhs, Optional<T> &rhs) noexcept(detail::is_noxcept_move_constructible<T>::value)
  {
    if(lhs.has_value() && rhs.has_value())
    {
      using std::swap;
      swap(*lhs, *rhs);
    }
    else if(lhs.has_value() && !rhs.has_value())
    {
      rhs.construct(std::move(*lhs));
      lhs.reset();
    }
    else if(!lhs.has_value() && rhs.has_value())
    {
      lhs.construct(std::move(*rhs));
      rhs.reset();
    }
  }  
};

#endif
