//
// Created by Zach Bortoff on 2019-11-25.
//

#include "defines.hpp"
#include "globals.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include "attack.hpp"

class AttackTester : public ::testing::Test {

};

TEST_F(AttackTester, BitboardToString) {

}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}