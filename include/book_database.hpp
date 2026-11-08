#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases

    // Ваш код здесь
    using value_type = Book;
    using iterator = BookContainer::iterator;
    using const_iterator = BookContainer::const_iterator;
    using size_type = BookContainer::size_type;
    using difference_type = BookContainer::difference_type;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    BookDatabase() = default;
    BookDatabase(std::initializer_list<Book> list) {
        for (const auto &book : list) {
            PushBack(book);
        }
    }

    // Учебное упрощение, чтобы не перетаскивать контейнер с авторами
    BookDatabase(const BookDatabase &) = delete;
    BookDatabase &operator=(const BookDatabase &) = delete;

    BookDatabase(BookDatabase &&) = delete;
    BookDatabase &operator=(BookDatabase &&) = delete;

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    // Standard container interface methods

    // Ваш код здесь

    iterator begin() { return books_.begin(); }
    iterator end() { return books_.end(); }
    const_iterator begin() const { return books_.begin(); }
    const_iterator end() const { return books_.end(); }

    bool empty() const { return books_.empty(); }
    size_type size() const { return books_.size(); }

    void PushBack(const Book &book) {
        auto it = authors_.find(book.m_Author);
        if (it == authors_.end()) {
            auto [iter, inserted] = authors_.emplace(book.m_Author);
            it = iter;
        }

        books_.push_back(book);
        books_.back().m_Author = *it;
    }
    void PushBack(Book &&book) {
        auto it = authors_.find(book.m_Author);
        if (it == authors_.end()) {
            auto [iter, inserted] = authors_.emplace(book.m_Author);
            it = iter;
        }

        book.m_Author = *it;
        books_.push_back(std::move(book));
    }

    template <typename... Args>
        requires std::constructible_from<Book, Args...>
    void EmplaceBack(Args &&...args) {
        PushBack(Book{std::forward<Args>(args)...});
    }

    std::span<const Book> GetBooks() const { return books_; }
    std::span<Book> GetBooks() { return books_; }
    const AuthorContainer &GetAuthors() { return authors_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

private:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        // Раскомментируйте, когда bookdb::BookDatabase поддержит интерфейсы, доступные стандартным контейнерам
        // (size/begin/...)

        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
