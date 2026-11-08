#include "heterogeneous_lookup.hpp"
#include <gtest/gtest.h>

using namespace bookdb;

TEST(Transparent, OrdersStrings) {
    TransparentStringLess less;

    EXPECT_TRUE(less("apple", "banana"));
    EXPECT_FALSE(less("banana", "apple"));
    EXPECT_FALSE(less("apple", "apple"));

    std::string cpp = "apple";
    EXPECT_TRUE(less(cpp, "banana"));
    EXPECT_FALSE(less("banana", cpp));
}

TEST(Transparent, SetAcceptsAndSorts) {
    std::set<std::string, TransparentStringLess> authors;

    authors.insert("George Orwell");
    authors.insert("Jane Austen");
    authors.insert("Aldous Huxley");

    std::vector<std::string> expected{"Aldous Huxley", "George Orwell", "Jane Austen"};
    EXPECT_EQ(std::vector<std::string>(authors.begin(), authors.end()), expected);
}

TEST(Transparent, HeterogeneousFind) {
    std::set<std::string, TransparentStringLess> authors;

    authors.insert("George Orwell");
    authors.insert("Jane Austen");

    constexpr std::string_view query = "George Orwell";
    auto it = authors.find(query);
    ASSERT_NE(it, authors.end());
    EXPECT_EQ(*it, "George Orwell");

    auto it2 = authors.find("Jane Austen");
    EXPECT_NE(it2, authors.end());
}