#include "book.hpp"
#include "book_database.hpp"

#include <concepts>
#include <gtest/gtest.h>
#include <vector>

using namespace bookdb;

using DB = BookDatabase<std::vector<Book>>;

static_assert(std::same_as<DB::value_type, Book>);
static_assert(std::same_as<DB::iterator, std::vector<Book>::iterator>);
static_assert(std::same_as<DB::const_iterator, std::vector<Book>::const_iterator>);
static_assert(std::same_as<DB::size_type, std::vector<Book>::size_type>);
static_assert(std::same_as<DB::difference_type, std::vector<Book>::difference_type>);

namespace {
constexpr std::string_view kTemplatesAuthor = "David Vandevoorde, Nicolai Josuttis, Douglas Gregor";
constexpr std::string_view kTemplatesTitle = "C++ Templates: The Complete Guide";
constexpr int kTemplatesYear = 2017;
constexpr Genre kTemplatesGenre = Genre::NonFiction;
constexpr double kTemplatesRating = 4.9;
constexpr int kTemplatesReadCount = 10;

constexpr std::string_view kWheelAuthor = "Robert Jordan";
constexpr std::string_view kWheelTitle = "The Eye of the World";
constexpr int kWheelYear = 1990;
constexpr Genre kWheelGenre = Genre::Fiction;
constexpr double kWheelRating = 4.8;
constexpr int kWheelReadCount = 20;

constexpr std::string_view kWheelSecondTitle = "The Great Hunt";
constexpr int kWheelSecondYear = 1990;
constexpr Genre kWheelSecondGenre = Genre::Fiction;
constexpr double kWheelSecondRating = 4.8;
constexpr int kWheelSecondReadCount = 15;

constexpr std::string_view kDraculaAuthor = "Bram Stoker";
constexpr std::string_view kDraculaTitle = "Dracula";
constexpr int kDraculaYear = 1897;
constexpr Genre kDraculaGenre = Genre::Fiction;
constexpr double kDraculaRating = 4.7;
constexpr int kDraculaReadCount = 30;
}  // namespace

TEST(BookDB, EmptyDB) {
    BookDatabase<std::vector<Book>> db;

    ASSERT_TRUE(db.empty());
    ASSERT_TRUE(db.size() == 0);
    ASSERT_TRUE(db.begin() == db.end());
}

TEST(BookDB, InitConstructor) {
    BookDatabase<std::vector<Book>> db{
        Book{kTemplatesTitle, kTemplatesAuthor, kTemplatesYear, kTemplatesGenre, kTemplatesRating, kTemplatesReadCount},
        Book{kWheelTitle, kWheelAuthor, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount},
        Book{kDraculaTitle, kDraculaAuthor, kDraculaYear, kDraculaGenre, kDraculaRating, kDraculaReadCount}};

    ASSERT_FALSE(db.empty());
    ASSERT_TRUE(db.size() == 3);
    ASSERT_TRUE(db.begin() != db.end());
    ASSERT_TRUE(db.GetAuthors().size() == 3);
}

TEST(BookDB, Clear) {
    BookDatabase<std::vector<Book>> db{
        Book{kWheelTitle, kWheelAuthor, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount}};

    db.Clear();

    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_EQ(db.begin(), db.end());
    EXPECT_TRUE(db.GetBooks().empty());
    EXPECT_TRUE(db.GetAuthors().empty());
}

TEST(BookDB, SameAuthor) {
    BookDatabase<std::vector<Book>> db{
        Book{kWheelTitle, kWheelAuthor, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount},
        Book{kWheelSecondTitle, kWheelAuthor, kWheelSecondYear, kWheelSecondGenre, kWheelSecondRating,
             kWheelSecondReadCount}};

    ASSERT_FALSE(db.empty());
    ASSERT_TRUE(db.size() == 2);
    ASSERT_TRUE(db.begin() != db.end());
    ASSERT_TRUE(db.GetAuthors().size() == 1);
}

TEST(BookDB, EmplaceBack) {
    BookDatabase<std::vector<Book>> db;
    db.EmplaceBack(kWheelTitle, kWheelAuthor, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount);

    ASSERT_EQ(db.size(), 1);
    const auto &book = db.GetBooks().front();

    EXPECT_EQ(book.m_Author, kWheelAuthor);
    EXPECT_EQ(book.m_Title, kWheelTitle);
    EXPECT_EQ(book.m_Year, kWheelYear);
    EXPECT_EQ(book.m_Genre, kWheelGenre);
    EXPECT_DOUBLE_EQ(book.m_Rating, kWheelRating);
    EXPECT_EQ(book.m_ReadCount, kWheelReadCount);
}

TEST(BookDB, ManyBooks) {
    DB db;
    constexpr int kBookCount = 15'000;
    constexpr int kAuthorCount = 10'001;

    for (int i = 0; i < kBookCount; ++i) {
        const auto title = std::format("Book {}", i);
        const auto author = std::format("Author {}", i % kAuthorCount);

        Book book{title, author, 1900 + i % 100, Genre::Fiction, 4.5, i};

        db.PushBack(std::move(book));
    }

    ASSERT_EQ(db.size(), kBookCount);
    ASSERT_EQ(db.GetAuthors().size(), kAuthorCount);

    const auto books = db.GetBooks();

    for (int i = 0; i < kBookCount; ++i) {
        const auto &book = books[i];
        const auto expected_author = std::format("Author {}", i % kAuthorCount);

        EXPECT_EQ(book.m_Title, std::format("Book {}", i));
        EXPECT_EQ(book.m_Author, expected_author);
        EXPECT_EQ(book.m_Year, 1900 + i % 100);
        EXPECT_EQ(book.m_Genre, Genre::Fiction);
        EXPECT_DOUBLE_EQ(book.m_Rating, 4.5);
        EXPECT_EQ(book.m_ReadCount, i);

        const auto it = db.GetAuthors().find(expected_author);
        ASSERT_NE(it, db.GetAuthors().end());
        EXPECT_EQ(book.m_Author.data(), it->data());
    }
}

TEST(BookDB, OwnsAuthorString) {
    BookDatabase<std::vector<Book>> db;

    {
        std::string author{kWheelAuthor};
        Book book{kWheelTitle, author, kWheelYear, kWheelGenre, kWheelRating, kWheelReadCount};

        db.PushBack(book);

        // Книга должна ссылаться на строку внутри базы.
        const auto it = db.GetAuthors().find(kWheelAuthor);
        ASSERT_NE(it, db.GetAuthors().end());
        ASSERT_EQ(db.GetBooks().front().m_Author.data(), it->data());

        author.assign(author.size(), 'X');
        EXPECT_EQ(db.GetBooks().front().m_Author, kWheelAuthor);
    }

    EXPECT_EQ(db.GetBooks().front().m_Author, kWheelAuthor);
}