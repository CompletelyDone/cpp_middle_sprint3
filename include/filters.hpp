#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {
constexpr auto YearBetween(int start, int end) {
    return [start, end](const Book &b) { return b.m_Year >= start && b.m_Year <= end; };
}
constexpr auto RatingAbove(double min_rating) {
    return [min_rating](const Book &b) { return b.m_Rating >= min_rating; };
}
constexpr auto GenreIs(Genre genre) {
    return [genre](const Book &b) { return b.m_Genre == genre; };
}

template <BookIterator It, BookSentinel<It> Sent, BookPredicate Pred>
auto filterBooks(It first, Sent last, Pred pred) {
    std::vector<std::reference_wrapper<const Book>> result;

    It index = first;
    while (index != last) {
        if (pred(*index))
            result.push_back(*index);

        index++;
    }

    return result;
}

template <BookPredicate... Preds>
constexpr auto all_of(Preds... preds) {
    return [... preds = preds](const Book &book) { return (preds(book) && ...); };
}

template <BookPredicate... Preds>
constexpr auto any_of(Preds... preds) {
    return [... preds = preds](const Book &book) { return (preds(book) || ...); };
}
}  // namespace bookdb