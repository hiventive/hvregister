/**
 * @file gtest_runner.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Feb, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Test runner for Register project
 */
#include <gtest/gtest.h>
#include <ctime>
#include <systemc>
#include <hv/configuration.h>

int sc_main(int argc, char* argv[]) {
	::hv::cfg::Broker myBroker("MyGlobalBroker");
	srand(time(NULL));
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	return ret;
}
