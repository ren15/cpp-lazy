#include "Lz/Chunks.hpp"
#include "catch2/catch.hpp"

#include <Lz/FunctionTools.hpp>
#include <list>

TEST_CASE("Chunks changing and creating elements", "[Chunks][Basic functionality]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    auto chunked = lz::chunks(v, 3);

    SECTION("Should be by ref") {
        auto it = chunked.begin();
        *it->begin() = 0;
        CHECK(v[0] == 0);
    }

    SECTION("Should be correct length") {
        CHECK(std::distance(chunked.begin(), chunked.end()) == 3);
    }
}

TEST_CASE("Chunks binary operations", "[Chunks][Binary ops]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto chunked = lz::chunks(v, 3);
    auto it = chunked.begin();

    SECTION("Operator++") {
        CHECK(*it->begin() == 1);
        CHECK(*it->end() == 4);
        ++it;
        CHECK(*it->begin() == 4);
        CHECK(*it->end() == 7);
        ++it;
        CHECK(*it->begin() == 7);
        CHECK(it->end() == chunked.end()->end());
    }

    SECTION("Operator++ (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto c = lz::chunks(lst, 3);
        auto iter = c.begin();
        CHECK(*iter->begin() == 1);
        CHECK(*iter->end() == 4);
        ++iter;
        CHECK(*iter->begin() == 4);
        CHECK(*iter->end() == 7);
        ++iter;
        CHECK(*iter->begin() == 7);
        CHECK(iter->end() == c.end()->end());
    }

    SECTION("Operator--") {
        auto end = chunked.end();
        --end;
        CHECK(*end->begin() == 6);
        CHECK(end->end() == chunked.end()->end());
        --end;
        CHECK(*end->begin() == 3);
        CHECK(*end->end() == 6);
        --end;
        CHECK(*end->begin() == 1);
        CHECK(*end->end() == 3);
    }

    SECTION("Operator-- (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto c = lz::chunks(lst, 3);
        auto iter = c.end();
        --iter;
        CHECK(*iter->begin() == 6);
        CHECK(iter->end() == c.end()->end());
        --iter;
        CHECK(*iter->begin() == 3);
        CHECK(*iter->end() == 6);
        --iter;
        CHECK(*iter->begin() == 1);
        CHECK(*iter->end() == 3);
    }

    SECTION("Operator== & operator!=") {
        CHECK(it == chunked.begin());
        CHECK(it != chunked.end());
        it = chunked.end();
        CHECK(it != chunked.begin());
        CHECK(it == chunked.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        auto beg = chunked.begin();
        auto end = chunked.end();

        CHECK(*(beg + 1)->begin() == 4);
        CHECK(*(beg + 2)->begin() == 7);
        CHECK((beg + 3)->begin() == chunked.end()->end());
        CHECK(*(beg + 1)->end() == 7);
        CHECK((beg + 2)->end() == chunked.end()->end());
        CHECK((beg + 3)->end() == chunked.end()->end());

        CHECK(*(end + -1)->begin() == 6);
        CHECK((end + -1)->end() == chunked.end()->end());
        CHECK(*(end + -2)->begin() == 3);
        CHECK(*(end + -2)->end() == 6);
        CHECK(*(end + -3)->begin() == 1);
        CHECK(*(end + -3)->end() == 3);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        auto end = chunked.end();
        CHECK(*(end - 1)->begin() == 6);
        CHECK((end - 1)->end() == chunked.end()->end());
    }

    SECTION("Operator-(Iterator)") {
        auto beg = chunked.begin();
        CHECK(std::distance(beg, chunked.end()) == 3);
        ++beg;
        CHECK(std::distance(beg, chunked.end()) == 2);
        ++beg;
        CHECK(std::distance(beg, chunked.end()) == 1);
        ++beg;
        CHECK(std::distance(beg, chunked.end()) == 0);
    }

    SECTION("Operator[]()") {
        auto beg = chunked.begin();
        CHECK(beg[0].toArray<3>() == std::array<int, 3>{1, 2, 3});
        CHECK(beg[1].toArray<3>() == std::array<int, 3>{4, 5, 6});
        CHECK(beg[2].toArray<2>() == std::array<int, 2>{7, 8});
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = chunked.begin();
        auto end = chunked.end();
        auto distance = std::distance(b, end);

        CHECK(b < end);
        CHECK(b + distance - 1 > end - distance);
        CHECK(b + distance - 1 <= end);
        CHECK(b + distance - 1 >= end - 1);
    }
}

TEST_CASE("Chunks to containers", "[Chunk][To container]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto chunked = lz::chunks(v, 3);

    SECTION("To array") {
        std::array<std::array<int, 3>, 3> arrays{};
        chunked.transformTo(arrays.begin(), [](auto&& chunk) { return chunk.template toArray<3>(); });

        std::array<std::array<int, 3>, 3> expected = { std::array<int, 3>{ 1, 2, 3 }, std::array<int, 3>{ 4, 5, 6 },
                                                       std::array<int, 3>{ 7, 8 } };

        CHECK(arrays == expected);
    }

    SECTION("To vector") {
        std::vector<std::vector<int>> vectors{};
        chunked.transformTo(std::back_inserter(vectors), [](auto&& chunk) { return chunk.toVector(); });

        std::vector<std::vector<int>> expected = { std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 4, 5, 6 },
                                                   std::vector<int>{ 7, 8 } };

        CHECK(vectors == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::list<int>> lists{};
        chunked.transformTo(std::inserter(lists, lists.begin()), [](auto&& chunk) { return chunk.template to<std::list>(); });

        std::list<std::list<int>> expected = { std::list<int>{ 1, 2, 3 }, std::list<int>{ 4, 5, 6 }, std::list<int>{ 7, 8 } };

        CHECK(lists == expected);
    }
}