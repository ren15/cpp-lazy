#include "Lz/FunctionTools.hpp"
#include "Lz/Range.hpp"
#include <catch2/catch.hpp>

#include <cctype>

TEST_CASE("Function tools") {
    std::vector<int> ints = {1, 2, 3, 4};
    std::vector<double> doubles = {1.2, 2.5, 3.3, 4.5};

    SECTION("Mean") {
        double avg = lz::mean(ints);
        CHECK(avg == Approx((1. + 2. + 3. + 4.) / 4.));
    }

    SECTION("Median") {
        double median = lz::median(doubles);
        CHECK(median == Approx((2.5 + 3.3) / 2.));

        doubles.push_back(3.3);
        median = lz::median(doubles);
        CHECK(median == Approx(3.3));
    }

	SECTION("Sum to") {
        CHECK(lz::sumTo(4) == 10);
        CHECK(lz::sumTo(5, 10) == 45);
        CHECK(lz::sumTo(4, 10) == 49);
        
        CHECK(lz::sumTo(-4) == -10);
        CHECK(lz::sumTo(-4, 3) == -4);
        CHECK(lz::sumTo(-4, -10) == -49);
    	
        CHECK(lz::sumTo(-10, -20) == -165);
    }

    SECTION("Unlines") {
        std::vector<std::string> strings = {"hello", "world", "what's", "up"};
        auto unlines = lz::unlines(strings).toString();
        CHECK(unlines == "hello\nworld\nwhat's\nup");
    }

    SECTION("Lines") {
        std::string string = "aa\nbb\nbb";
        auto lines = lz::lines<std::string>(string).toVector();
        CHECK(lines == std::vector<std::string>{"aa", "bb", "bb"});
    }

#if (!defined(LZ_HAS_CXX_17)) && (!defined(LZ_HAS_CXX_20))
    SECTION("Transform accumulate") {
        std::vector<std::string> s = {"hello", "world", "!"};
        size_t totalSize = lz::transAccumulate(s, static_cast<std::size_t>(0), [](std::size_t i, const std::string& s) {
            return i + s.size();
        });
        CHECK(totalSize == 11);
    }
#endif

    SECTION("Pairwise") {
        auto x = lz::pairwise(ints).toVector();
        CHECK(x == std::vector<std::tuple<int, int>>{std::make_tuple(1, 2), std::make_tuple(2, 3), std::make_tuple(3, 4)});
    }

    SECTION("As") {
        auto floats = lz::as<float>(ints).toVector();
        CHECK(std::is_same<typename decltype(floats)::value_type, float>::value);
        CHECK(floats == std::vector<float>{1., 2., 3., 4.});
    }

    SECTION("First or default") {
        std::vector<std::string> s = {"hello world!", "what's up"};
        std::string toFind = "hel";
        std::string def = "default";

        toFind = lz::firstOrDefault(s, toFind, def);
        CHECK(toFind == "default");

        toFind = "hello world!";
        toFind = lz::firstOrDefault(s, toFind, def);
        CHECK(toFind == "hello world!");

        def = ' ';
        toFind = lz::firstOrDefaultIf(s, [](const std::string& s) {
            return s.find('!') != std::string::npos;
        }, def);
        CHECK(toFind == "hello world!");

        toFind = lz::firstOrDefaultIf(s, [](const std::string& s) {
            return s.find('z') != std::string::npos;
        }, "default");
        CHECK(toFind == "default");
    }

    SECTION("Last or default") {
        std::vector<std::string> s = {"hello 'world", "what's up"};
        std::string toFind = "hel";
        std::string def = "default";

        toFind = lz::lastOrDefault(s, toFind, def);
        CHECK(toFind == def);

        toFind = lz::lastOrDefaultIf(s, [](const std::string& s) {
            return s.find('\'') != std::string::npos;
        }, def);
        CHECK(toFind == "what's up");

        toFind = lz::lastOrDefaultIf(s, [](const std::string& s) {
            return lz::contains(s, 'q');
        }, def);
        CHECK(toFind == def);
    }

    SECTION("Position") {
        std::vector<char> c = {'a', 'b', 'c', 'd'};
        std::size_t pos = lz::indexOf(c, 'b');
        CHECK(pos == 1);

        pos = lz::indexOf(c, 'e');
        CHECK(pos == lz::npos);

        std::vector<std::string> strings = {"hello", "world"};
        pos = lz::indexOfIf(strings, [](const std::string& s) {
            return s.find('o') != std::string::npos;
        });
        CHECK(pos == 0);

        pos = lz::indexOfIf(strings, [](const std::string& s) {
            return s.find('q') != std::string::npos;
        });
        CHECK(pos == lz::npos);
    }

    SECTION("FilterMap") {
        std::string s = "123,d35dd";
        auto f = lz::filterMap(s, [](const char c) { return static_cast<bool>(std::isdigit(c)); },
                               [](const char c) { return static_cast<int>(c - '0'); });
        auto expected = { 1, 2, 3, 3, 5 };
        CHECK(std::equal(f.begin(), f.end(), expected.begin()));
    }

    SECTION("To string func") {
        std::vector<int> v = {1, 2, 3, 4, 5};
        auto dummy = lz::map(v, [](int i) { return i; });

        CHECK(dummy.toString() == "12345");
        CHECK(dummy.toString(" ") == "1 2 3 4 5");
        CHECK(dummy.toString(", ") == "1, 2, 3, 4, 5");
    }

    SECTION("String replace") {
        std::string myString = "picture.jpg";
        lz::strReplace(myString, ".jpg", ".jpeg");
        CHECK(myString == "picture.jpeg");
        CHECK(myString.length() == std::strlen("picture.jpeg"));

        myString = "picture.png.png";
        lz::strReplaceAll(myString, ".png", ".jpeg");
        CHECK(myString == "picture.jpeg.jpeg");
        CHECK(myString.length() == std::strlen("picture.jpeg.jpeg"));
    }

	SECTION("Reverse") {
        std::string s = "hello";
        CHECK(lz::reverse(s).toString() == "olleh");
    }

#ifdef LZ_HAS_CXX_17
    SECTION("Concat as string view") {
        std::string world = "world ";

        auto concat = lz::concatAsStringView("hello ", world, "what's ", "Up");
        CHECK(concat.toString() == "hello world what's Up");
    }
#endif

    SECTION("Last, first, length, isEmpty") {
        std::vector<int> vec = {1, 3, 5, 7, 9};
        auto filterEven = lz::filter(vec, [](const int i) { return i % 2 == 0; });
        auto filterUneven = lz::filter(vec, [](const int i) { return i % 2 != 0; });

        CHECK(lz::length(filterEven) == 0);
        CHECK(lz::isEmpty(filterEven));
        CHECK(static_cast<std::size_t>(lz::length(filterUneven)) == vec.size());

        CHECK(lz::first(filterUneven) == 1);
        CHECK(lz::last(filterUneven) == 9);

        CHECK(lz::firstOr(filterEven, 10) == 10);
        CHECK(lz::lastOr(filterEven, 10) == 10); 

        CHECK(lz::firstOr(filterUneven, 10) == 1);
        CHECK(lz::lastOr(filterUneven, 10) == 9);

        CHECK(!lz::hasOne(filterUneven));
        CHECK(lz::hasMany(filterUneven));

        std::string splitter = "hello world";
        auto s = lz::split(splitter, " ").toVector();
    }

    SECTION("Contains") {
        std::string s = "hello";
        CHECK(lz::contains(s, 'h'));
        CHECK(!lz::contains(s, 'x'));
    }

    SECTION("select") {
        std::array<int, 10> range = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::function<bool(int)> even = [](int i) { return i % 2 == 0; };
        auto selectors = lz::map(range, std::move(even));
        auto selected = lz::select(range, std::move(selectors));
        CHECK(selected.toVector<int>() == std::vector<int>{0, 2, 4, 6, 8});
    }

    SECTION("Zip with") {
        std::vector<int> v = {1,2,3};
        std::vector<int> v2 = {1,2,3};

        auto zipper = lz::zipWith([](int& a, int b) {
            int tmp = a;
            a = 0;
            return tmp + b;
        }, v, v2).toVector();

        CHECK(zipper == std::vector<int>{2,4,6});
        CHECK(v == std::vector<int>{0,0,0}); // ref check
    }

    SECTION("Trimming") {
        std::string toTrim = "\n\n  Hello world    \t\t";
        using IsSpace = int(*)(int);
        auto trimming = lz::trim(toTrim, static_cast<IsSpace>(&std::isspace), static_cast<IsSpace>(&std::isspace));
        CHECK(trimming.toString() == "Hello world");
    }
}
