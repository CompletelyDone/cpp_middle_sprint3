#include "comparators.hpp"
#include "statsistics.hpp"

#include <gtest/gtest.h>
#include <string_view>
#include <unordered_set>
#include <vector>

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

TEST(Statistics, AuthorHistogram) {
    const auto db = MakeDatabase();

    const auto histogram = buildAuthorHistogramFlat(db);

    ASSERT_EQ(histogram.size(), 3);
    ASSERT_TRUE(histogram.contains(kWheelAuthor));
    ASSERT_TRUE(histogram.contains(kDraculaAuthor));

    EXPECT_EQ(histogram.at(kWheelAuthor), 2);
    EXPECT_EQ(histogram.at(kDraculaAuthor), 1);
}

TEST(Statistics, GenreRatings) {
    const auto db = MakeDatabase();

    const auto calc = calculateGenreRatings(db.begin(), db.end());

    ASSERT_EQ(calc.size(), 2);
    ASSERT_TRUE(calc.contains(Genre::Fiction));
    ASSERT_TRUE(calc.contains(Genre::NonFiction));

    const double expected = (2.0 * kWheelRating + kDraculaRating) / 3.0;
    EXPECT_DOUBLE_EQ(calc.at(Genre::Fiction), expected);

    EXPECT_DOUBLE_EQ(calc.at(Genre::NonFiction), kTemplatesRating);
}

TEST(Statistics, AuthorHistogramEmpty) {
    const BookDatabase<std::vector<Book>> db;
    EXPECT_TRUE(buildAuthorHistogramFlat(db).empty());
}

TEST(Statistics, AuthorHistogramCustomComparator) {
    const auto db = MakeDatabase();
    const auto histogram = buildAuthorHistogramFlat(db, std::greater<std::string_view>{});

    ASSERT_EQ(histogram.size(), 3);
    EXPECT_EQ(histogram.begin()->first, kWheelAuthor);
    EXPECT_EQ(histogram.at(kWheelAuthor), 2);
    EXPECT_EQ(histogram.at(kDraculaAuthor), 1);
    EXPECT_EQ(histogram.at(kTemplatesAuthor), 1);
}

TEST(Statistics, GenreRatingsEmpty) {
    const BookDatabase<std::vector<Book>> db;
    EXPECT_TRUE(calculateGenreRatings(db.begin(), db.end()).empty());
}

TEST(Statistics, GenreRatingsWithDifferentSentinel) {
    const auto db = MakeDatabase();
    const auto first = std::counted_iterator(db.begin(), static_cast<std::ptrdiff_t>(db.size()));
    const auto ratings = calculateGenreRatings(first, std::default_sentinel);

    ASSERT_EQ(ratings.size(), 2);
    ASSERT_TRUE(ratings.contains(Genre::Fiction));
    ASSERT_TRUE(ratings.contains(Genre::NonFiction));
    EXPECT_DOUBLE_EQ(ratings.at(Genre::Fiction), (2.0 * kWheelRating + kDraculaRating) / 3.0);
    EXPECT_DOUBLE_EQ(ratings.at(Genre::NonFiction), kTemplatesRating);
}

TEST(Statistics, AverageRating) {
    const auto db = MakeDatabase();
    const double expected = (2.0 * kWheelRating + kDraculaRating + kTemplatesRating) / 4.0;
    EXPECT_DOUBLE_EQ(calculateAverageRating(db), expected);
}

TEST(Statistics, AverageRatingEmpty) {
    const BookDatabase<std::vector<Book>> db;
    EXPECT_DOUBLE_EQ(calculateAverageRating(db), 0.0);
}

TEST(Statistics, AverageRatingSingleBook) {
    const BookDatabase<std::vector<Book>> db{
        Book{kDraculaTitle, kDraculaAuthor, kDraculaYear, kDraculaGenre, kDraculaRating, kDraculaReadCount}};
    EXPECT_DOUBLE_EQ(calculateAverageRating(db), kDraculaRating);
}

TEST(Statistics, RandomBooksEmpty) {
    const BookDatabase<std::vector<Book>> db;
    EXPECT_TRUE(sampleRandomBooks(db, 3).empty());
}

TEST(Statistics, RandomBooks) {
    const auto db = MakeDatabase();
    const auto sampled = sampleRandomBooks(db, 2);

    ASSERT_EQ(sampled.size(), 2);
    EXPECT_NE(&sampled[0].get(), &sampled[1].get());

    for (const auto &ref : sampled) {
        const auto it = std::find_if(db.begin(), db.end(), [&ref](const Book &book) { return &book == &ref.get(); });
        EXPECT_NE(it, db.end());
    }
}

TEST(Statistics, TopBooksByRating) {
    auto db = MakeDatabase();
    const auto top = getTopNBy(db, 2, comp::LessByRating{});

    ASSERT_EQ(top.size(), 2);
    EXPECT_EQ(top[0].get().m_Title, kTemplatesTitle);
    EXPECT_DOUBLE_EQ(top[0].get().m_Rating, kTemplatesRating);
    EXPECT_DOUBLE_EQ(top[1].get().m_Rating, kWheelRating);
    EXPECT_EQ(&top[0].get(), &db.GetBooks()[0]);
    EXPECT_EQ(&top[1].get(), &db.GetBooks()[1]);
    EXPECT_EQ(db.size(), 4);
}

TEST(Statistics, TopBooksByYear) {
    auto db = MakeDatabase();
    const auto top = getTopNBy(db, 1, comp::LessByYear{});

    ASSERT_EQ(top.size(), 1);
    EXPECT_EQ(top.front().get().m_Title, kTemplatesTitle);
}

TEST(Statistics, TopBooksEmpty) {
    BookDatabase<std::vector<Book>> db;
    EXPECT_TRUE(getTopNBy(db, 3, comp::LessByRating{}).empty());
}

TEST(Statistics, RandomBooksZeroCount) {
    const auto db = MakeDatabase();

    EXPECT_TRUE(sampleRandomBooks(db, 0).empty());
}

TEST(Statistics, RandomBooksCountExceedsSize) {
    const auto db = MakeDatabase();
    const auto sampled = sampleRandomBooks(db, db.size() + 5);

    ASSERT_EQ(sampled.size(), db.size());

    for (const auto &book : db) {
        const auto count =
            std::count_if(sampled.begin(), sampled.end(), [&book](const auto &ref) { return &ref.get() == &book; });

        EXPECT_EQ(count, 1);
    }
}

TEST(Statistics, TopBooksZeroCount) {
    auto db = MakeDatabase();

    EXPECT_TRUE(getTopNBy(db, 0, comp::LessByRating{}).empty());
}

TEST(Statistics, TopBooksCountExceedsSize) {
    auto db = MakeDatabase();
    const auto top = getTopNBy(db, db.size() + 5, comp::LessByRating{});

    ASSERT_EQ(top.size(), db.size());

    for (std::size_t i = 0; i < top.size(); ++i) {
        EXPECT_EQ(&top[i].get(), &db.GetBooks()[i]);

        if (i > 0) {
            EXPECT_GE(top[i - 1].get().m_Rating, top[i].get().m_Rating);
        }
    }
}