#include "book.hpp"
#include "comparators.hpp"
#include <gtest/gtest.h>

TEST(Comparators, LessByAuthor) {
    bookdb::Book a{"War and Peace", "Leo Tolstoy", 1869, bookdb::Genre::Fiction, 4.5, 10};
    bookdb::Book b{"Pride and Prejudice", "Jane Austen", 1813, bookdb::Genre::Fiction, 4.3, 5};

    bookdb::comp::LessByAuthor cmp;

    EXPECT_FALSE(cmp(a, b));
    EXPECT_TRUE(cmp(b, a));
}

TEST(Comparators, LessByYear) {
    bookdb::Book a{"War and Peace", "Leo Tolstoy", 1869, bookdb::Genre::Fiction, 4.5, 10};
    bookdb::Book b{"Pride and Prejudice", "Jane Austen", 1813, bookdb::Genre::Fiction, 4.3, 5};

    bookdb::comp::LessByYear cmp;

    EXPECT_FALSE(cmp(a, b));
    EXPECT_TRUE(cmp(b, a));
}