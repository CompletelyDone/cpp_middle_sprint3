#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

// Ваш код для constexpr преобразования строк в enum::Genre и наоборот здесь
constexpr Genre GenreFromString(std::string_view s) {
    // Ваш код здесь
    // clang-format off
    if (s == "Fiction")         return Genre::Fiction;
    else if (s == "NonFiction") return Genre::NonFiction;
    else if (s == "SciFi")      return Genre::SciFi;
    else if (s == "Biography")  return Genre::Biography;
    else if (s == "Mystery")    return Genre::Mystery;
    else if (s == "Unknown")    return Genre::Unknown;
    // clang-format on

    return Genre::Unknown;
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string m_Title;
    std::string_view m_Author;

    int m_Year;
    Genre m_Genre;
    double m_Rating;
    int m_ReadCount;

    // Ваш код для конструкторов здесь
    constexpr Book(std::string_view title, std::string_view author, int year, Genre genre, double rating, int readCount)
        : m_Title(title), m_Author(author), m_Year(year), m_Genre(genre), m_Rating(rating), m_ReadCount(readCount) {}
    constexpr Book(std::string_view title, std::string_view author, int year, std::string_view genre, double rating,
                   int readCount)
        : m_Title(title), m_Author(author), m_Year(year), m_Genre(GenreFromString(genre)), m_Rating(rating),
          m_ReadCount(readCount) {}
};
}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        std::string genre_str;

        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

// Ваш код для std::formatter<Book> здесь
template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &b, FormatContext &fc) const {
        return format_to(fc.out(), "{} {} {} {} {} {}", b.m_Author, b.m_Title, b.m_Year, b.m_Rating, b.m_Genre,
                         b.m_ReadCount);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

}  // namespace std
