#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_Author < rhs.m_Author; }
};
struct LessByTitle {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_Title < rhs.m_Title; }
};
struct LessByRating {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_Rating < rhs.m_Rating; }
};
struct LessByGenre {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_Genre < rhs.m_Genre; }
};
struct LessByReadCount {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_ReadCount < rhs.m_ReadCount; }
};
struct LessByYear {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_Year < rhs.m_Year; }
};
struct LessByPopularity {
    constexpr bool operator()(const Book &lhs, const Book &rhs) { return lhs.m_ReadCount < rhs.m_ReadCount; }
};

}  // namespace bookdb::comp