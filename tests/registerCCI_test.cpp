/**
 * @file registerCCI_test.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register test
 */

#include "common/hvutils.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "gtest/gtest.h"
#include "register/register.h"
#include "register/register_cci.h"

using namespace ::hv::common;
using namespace ::hv::reg;

class RegisterCCITest: public ::testing::Test {
};

TEST_F(RegisterCCITest, InstantiationTest) {
	// Instantiating RegisterCCI instance inside a register

}
