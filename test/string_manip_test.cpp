//
// Created by Zach Bortoff on 2020-06-03.
//

#include <gtest/gtest.h>
#include "string_manip.hpp"

class StringManipTester : public ::testing::Test {
protected:
    std::string test1{"Hello World!"}; char delim1{' '};
    std::string test2{"1.2,3.3,4.4,8.4"}; char delim2{','};
    std::string test3{"the quick brown fox jumped over the lazy dog"}; char delim3{' '};
    std::string test4{"012321-23231-12312415"}; char delim4{'-'};
};

TEST_F(StringManipTester, Split) {
    std::vector<std::string> test1_ans({"Hello", "World!"});
    EXPECT_EQ(split(test1, delim1), test1_ans);
    EXPECT_EQ(split(test1), test1_ans);
    EXPECT_EQ(split(test2, delim2), std::vector<std::string>({"1.2", "3.3", "4.4", "8.4"}));
    EXPECT_EQ(split(test3, delim3),
              std::vector<std::string>({"the", "quick", "brown", "fox", "jumped", "over", "the", "lazy", "dog"}));
    EXPECT_EQ(split(test4, delim4), std::vector<std::string>({"012321", "23231", "12312415"}));
}

TEST_F(StringManipTester, SwapUpperAndLowerCase) {
    std::string t1{"Hello World!"};
    auto a1 = swap_lower_upper_case(t1);
    EXPECT_EQ(a1, "hELLO wORLD!");
    std::string t2{"8/8/p1p5/1p5p/1P5p/8/PPP2K1p/4R1rk"};
    auto a2 = swap_lower_upper_case(t2);
    EXPECT_EQ(a2, "8/8/P1P5/1P5P/1p5P/8/ppp2k1P/4r1RK");
    std::string t3{"1q1k4/2Rr4/8/2Q3K1/8/8/8/8"};
    auto a3 = swap_lower_upper_case(t3);
    EXPECT_EQ(a3, "1Q1K4/2rR4/8/2q3k1/8/8/8/8");
    std::string t4{"7k/5K2/5P1p/3p4/6P1/3p4/8/8"};
    auto a4 = swap_lower_upper_case(t4);
    EXPECT_EQ(a4, "7K/5k2/5p1P/3P4/6p1/3P4/8/8");
    std::string t5{"8/6B1/p5p1/Pp4kp/1P5r/5P1Q/4q1PK/8"};
    auto a5 = swap_lower_upper_case(t5);
    EXPECT_EQ(a5, "8/6b1/P5P1/pP4KP/1p5R/5p1q/4Q1pk/8");
    std::string t6{"8/8/1p1r1k2/p1pPN1p1/P3KnP1/1P6/8/3R4"};
    auto a6 = swap_lower_upper_case(t6);
    EXPECT_EQ(a6, "8/8/1P1R1K2/P1Ppn1P1/p3kNp1/1p6/8/3r4");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}