#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T t) { t.size(); };

template <typename T>
concept BookIterator = requires(T t) {
    ++t;
    { *t } -> std::convertible_to<Book>;
};

template <typename S, typename I>
concept BookSentinel = true;

template <typename P>
concept BookPredicate = requires(P p, Book book) {
    { p(book) } -> std::is_invocable<P, Book>;
};

template <typename C>
concept BookComparator = requires(Book a, Book b, C cmp) {
    { cmp(a, b) } -> std::same_as<bool>;
};

}  // namespace bookdb