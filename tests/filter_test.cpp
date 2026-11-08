#include "book_database.hpp"
#include "filters.hpp"
#include <gtest/gtest.h>

using namespace bookdb;

namespace {
constexpr std::string_view kWheelAuthor = "Robert Jordan";
constexpr std::string_view kWheelTitle = "The Eye of the World";
constexpr std::string_view kWheelSecondTitle = "The Great Hunt";
constexpr int kWheelYear = 1990;
constexpr Genre kWheelGenre = Genre::Fiction;
constexpr double kWheelRating = 4.8;
constexpr int kWheelReadCount = 20;

constexpr std::string_view kDraculaAuthor = "Bram Stoker";
constexpr std::string_view kDraculaTitle = "Dracula";
constexpr int kDraculaYear = 1897;
constexpr Genre kDraculaGenre = Genre::Fiction;
constexpr double kDraculaRating = 4.7;
constexpr int kDraculaReadCount = 30;

constexpr std::string_view kTemplatesAuthor = "David Vandevoorde, Nicolai Josuttis, Douglas Gregor";
constexpr std::string_view kTemplatesTitle = "C++ Templates: The Complete Guide";
constexpr int kTemplatesYear = 2017;
constexpr Genre kTemplatesGenre = Genre::NonFiction;
constexpr double kTemplatesRating = 4.9;
constexpr int kTemplatesReadCount = 10;

BookDatabase<std::vector<Book>> MakeDatabase() {
    return BookDatabase<std::vector<Book>>{
        Book{kWheelTitle, kWheelAuthor, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount},
        Book{kDraculaTitle, kDraculaAuthor, kDraculaYear, kDraculaGenre, kDraculaRating, kDraculaReadCount},
        Book{kTemplatesTitle, kTemplatesAuthor, kTemplatesYear, kTemplatesGenre, kTemplatesRating, kTemplatesReadCount},
        Book{kWheelSecondTitle, kWheelAuthor, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount}};
}
}  // namespace

TEST(Filters, YearBetween) {
    const auto db = MakeDatabase();

    const auto filtered = filterBooks(db.begin(), db.end(), YearBetween(1897, 1990));

    ASSERT_EQ(filtered.size(), 3);
    EXPECT_EQ(filtered[0].get().m_Title, kWheelTitle);
    EXPECT_EQ(filtered[1].get().m_Title, kDraculaTitle);
    EXPECT_EQ(filtered[2].get().m_Title, kWheelSecondTitle);
}

TEST(Filters, RatingAbove) {
    const auto db = MakeDatabase();
    const auto pred = RatingAbove(kWheelRating);

    EXPECT_FALSE(pred(db.GetBooks()[1]));
    EXPECT_TRUE(pred(db.GetBooks()[0]));
    EXPECT_TRUE(pred(db.GetBooks()[2]));
}

TEST(Filters, GenreIs) {
    const auto db = MakeDatabase();
    const auto pred = GenreIs(Genre::NonFiction);

    EXPECT_TRUE(pred(db.GetBooks()[2]));
    EXPECT_FALSE(pred(db.GetBooks()[0]));
}

TEST(Filters, AllOf) {
    const auto db = MakeDatabase();
    const auto pred = bookdb::all_of(YearBetween(1900, 2000), RatingAbove(4.8));

    EXPECT_TRUE(pred(db.GetBooks()[0]));
    EXPECT_FALSE(pred(db.GetBooks()[2]));  // Не проходит по году
    EXPECT_TRUE(bookdb::all_of()(db.GetBooks()[0]));
}

TEST(Filters, AnyOf) {
    const auto db = MakeDatabase();
    const auto pred = bookdb::any_of(GenreIs(Genre::NonFiction), YearBetween(1900, 2000));

    EXPECT_TRUE(pred(db.GetBooks()[0]));   // Подходит год
    EXPECT_TRUE(pred(db.GetBooks()[2]));   // Подходит жанр
    EXPECT_FALSE(pred(db.GetBooks()[1]));  // Не подходит ничего
    EXPECT_FALSE(bookdb::any_of()(db.GetBooks()[0]));
}

TEST(Filters, EmptyRange) {
    const BookDatabase<std::vector<Book>> db;

    const auto filtered = filterBooks(db.begin(), db.end(), GenreIs(Genre::Fiction));

    EXPECT_TRUE(filtered.empty());
}

TEST(Filters, NoMatches) {
    const auto db = MakeDatabase();

    const auto filtered = filterBooks(db.begin(), db.end(), GenreIs(Genre::Biography));

    EXPECT_TRUE(filtered.empty());
}