#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T t, Book b) {
    t.size();
    t.begin();
    t.end();
    t.empty();
    t.clear();
    t.back();
    t.push_back(b);
    requires std::same_as<typename T::value_type, Book>;
    requires std::contiguous_iterator<typename T::iterator>;  // Для работы с std::span нужен непрерывный контейнер
};

template <typename T>
concept BookIterator = requires(T t) {
    requires std::is_lvalue_reference_v<std::iter_reference_t<T>>;
    requires std::same_as<std::remove_cvref_t<std::iter_reference_t<T>>, Book>;
    requires std::input_iterator<T>;
};

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = requires(P p, Book book) {
    { p(book) } -> std::convertible_to<bool>;
};

template <typename C>
concept BookComparator = requires(Book a, Book b, C cmp) {
    { cmp(a, b) } -> std::same_as<bool>;
};

}  // namespace bookdb