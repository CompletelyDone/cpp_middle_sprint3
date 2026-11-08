#pragma once

#include <algorithm>
#include <cstddef>
#include <flat_map>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string_view>

#include "book.hpp"
#include "book_database.hpp"

#include <print>
#include <unordered_set>
#include <utility>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string_view, std::size_t, Comparator> result(comp);

    // Вопрос. Почему такой код даёт ошибку компиляции?
    // std::for_each(cont.begin(), cont.end(), [&result](const Book &book) { result[book.m_Author]++; });
    std::for_each(cont.begin(), cont.end(), [&result](const Book &book) {
        auto [it, inserted] = result.try_emplace(book.m_Author, 0);
        ++it->second;
    });

    return result;
}

template <BookIterator It, BookSentinel<It> Sent>
auto calculateGenreRatings(It first, Sent last) {
    std::flat_map<Genre, std::pair<double, std::size_t>> map;
    std::flat_map<Genre, double> result;

    auto iter = first;
    while (iter != last) {
        auto [it, inserted] = map.try_emplace((*iter).m_Genre);
        it->second.first += (*iter).m_Rating;
        ++it->second.second;

        iter++;
    }

    std::for_each(map.begin(), map.end(), [&result](const auto &entry) {
        const auto &[genre, stats] = entry;
        const auto &[sumRating, count] = stats;

        result.emplace(genre, sumRating / count);
    });

    return result;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &books) {
    if (books.empty())
        return 0;

    const auto sum = std::accumulate(books.begin(), books.end(), 0.0,
                                     [](double sum, const Book &book) { return sum + book.m_Rating; });
    return sum / books.size();
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &cont, size_t num) {
    std::vector<std::reference_wrapper<const Book>> out;

    std::sample(cont.begin(), cont.end(), std::back_inserter(out), num, std::mt19937{std::random_device{}()});

    return out;
}

template <BookContainerLike T, typename Comparator>
auto getTopNBy(BookDatabase<T> &cont, size_t n, Comparator comp) {
    std::vector<std::reference_wrapper<const Book>> result;

    // Если кол-во запрошенных элементов больше размера бд, вернем всё чо есть
    if (n > cont.size())
        n = cont.size();
    std::partial_sort(cont.begin(), cont.begin() + n, cont.end(), [&comp](const Book &first, const Book &second) {
        return comp(second, first);  // Чтобы развернуть список
    });

    for (size_t i = 0; i < n; ++i) {
        result.push_back(*(cont.begin() + i));
    }

    return result;
}

}  // namespace bookdb
