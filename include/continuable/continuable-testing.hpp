
/**
  Copyright(c) 2015 - 2017 Denis Blank <denis.blank at outlook dot com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files(the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions :

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
**/

#ifndef CONTINUABLE_TESTING_HPP_INCLUDED__
#define CONTINUABLE_TESTING_HPP_INCLUDED__

#include "gtest/gtest.h"

#include "continuable/continuable-base.hpp"

namespace cti {
inline namespace abi_v1 {
namespace detail {
namespace testing {
template <typename C> void expect_async_completion(C&& continuable) {
  bool called = false;
  std::forward<C>(continuable).then([&](auto&&... args) {
    ASSERT_FALSE(called);
    called = true;

    // Workaround for our known GCC bug.
    util::unused(std::forward<decltype(args)>(args)...);
  });
  EXPECT_TRUE(called);
}

template <typename C, typename V>
void expect_async_validation(C&& continuable, V&& validator) {
  expect_async_completion(
      std::forward<C>(continuable)
          .then([validator =
                     std::forward<V>(validator)](auto&&... args) mutable {

            validator(std::forward<decltype(args)>(args)...);
          }));
}

/// Expects that the continuable is finished with the given arguments
template <typename C, typename V, typename... Args>
void expect_async_binary_validation(C&& continuable, V&& validator,
                                    Args&&... expected) {
  expect_async_validation(std::forward<C>(continuable), [
    expected_pack = std::make_tuple(std::forward<Args>(expected)...),
    validator = std::forward<V>(validator)
  ](auto&&... args) mutable {

    auto actual_pack = std::make_tuple(std::forward<decltype(args)>(args)...);

    auto size = util::pack_size_of(util::identity_of(expected_pack));

    static_assert(size.value == std::tuple_size<decltype(actual_pack)>::value,
                  "Async completion handler called with a different count "
                  "of arguments!");

    util::static_for_each_in(
        std::make_index_sequence<size.value>{}, [&](auto current) mutable {
          auto expected = std::get<current.value>(std::move(expected_pack));
          auto actual = std::get<current.value>(std::move(actual_pack));
          (void)current;

          validator(expected, actual);
        });
  });
}

template <typename C, typename... Args>
void expect_async_result(C&& continuable, Args&&... expected) {
  expect_async_binary_validation(
      std::forward<C>(continuable),
      [](auto&& expected, auto&& actual) { EXPECT_EQ(expected, actual); },
      std::forward<Args>(expected)...);
}

template <typename C, typename... Args>
void expect_async_types(C&& continuable, util::identity<Args...> expected) {
  expect_async_validation(
      std::forward<C>(continuable), [&](auto... actualPack) {
        auto actual = util::identity<decltype(actualPack)...>{};
        util::unused(expected, actual,
                     std::forward<decltype(actualPack)>(actualPack)...);

        static_assert(
            std::is_same<std::decay_t<decltype(expected)>,
                         std::decay_t<decltype(actual)>>::value,
            "The called arguments don't match with the expected ones!");
      });
}
} // end namespace testing
} // end namespace detail
} // end inline namespace abi_...
} // end namespace cti

/// Expects the final callback of the given continuable to be called
/// with any result.
#define EXPECT_ASYNC_COMPLETION(CONTINUABLE)                                   \
  cti::detail::testing::expect_async_completion(CONTINUABLE);

/// Expects the continuation to be called and forwards it's arguments to
/// the given validator which can then do assertions on the result.
#define EXPECT_ASYNC_VALIDATION(CONTINUABLE, VALIDATOR)                        \
  cti::detail::testing::expect_async_validation(CONTINUABLE, VALIDATOR);

/// Expects the continuation to be called and forwards it's arguments to
/// the given validator which can then do assertions on the result:
///
///   auto validator = [](auto expected, auto actual) {
///     // ...
///   };
///
/// The validator is called for every expecting and actual result.
#define EXPECT_ASYNC_BINARY_VALIDATION(CONTINUABLE, ...)                       \
  cti::detail::testing::expect_async_binary_validation(CONTINUABLE,            \
                                                       __VA_ARGS__);

/// Expects that the continuable is finished with the given result
#define EXPECT_ASYNC_RESULT(...)                                               \
  cti::detail::testing::expect_async_result(__VA_ARGS__);

/// Expects that the continuable is finished with the given type of arguments
/// without validating it against equality.
#define EXPECT_ASYNC_TYPES(CONTINUABLE, ...)                                   \
  cti::detail::testing::expect_async_types(                                    \
      CONTINUABLE, cti::detail::util::identity<__VA_ARGS__>{})

#endif // CONTINUABLE_TESTING_HPP_INCLUDED__
