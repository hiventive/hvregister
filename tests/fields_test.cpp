/**
 * @file fields_test.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Fields test
 */

#include "register/field/fields.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "gtest/gtest.h"

using namespace ::hv::common;
using namespace ::hv::reg;

class FieldsTest: public ::testing::Test {
protected:
	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};

TEST_F(FieldsTest, AddAndGetFieldsTest) {
	Fields fields;

	// Adding fields
	fields.add("Field_1", 0, 3, "Some Field 1", RO);
	fields.add("Field_2", 4, 4, "Some Field 2", RW);
	fields.add("Field_3", 10, 15, "Some Field 3", WO);
	fields.add("Field_4", 5, 9, "Some Field 4", NA);

	std::size_t indLow;
	std::size_t indHigh;
	hvrwmode_t mode;

	ASSERT_TRUE(fields.get("Field_1", &indLow, &indHigh, &mode));
	ASSERT_EQ(indLow, std::size_t(0));
	ASSERT_EQ(indHigh, std::size_t(3));
	ASSERT_EQ(mode, RO);

	ASSERT_TRUE(fields.get("Field_2", &indLow, &indHigh, &mode));
	ASSERT_EQ(indLow, std::size_t(4));
	ASSERT_EQ(indHigh, std::size_t(4));
	ASSERT_EQ(mode, RW);

	ASSERT_TRUE(fields.get("Field_3", &indLow, &indHigh, &mode));
	ASSERT_EQ(indLow, std::size_t(10));
	ASSERT_EQ(indHigh, std::size_t(15));
	ASSERT_EQ(mode, WO);

	ASSERT_TRUE(fields.get("Field_4", &indLow, &indHigh, &mode));
	ASSERT_EQ(indLow, std::size_t(5));
	ASSERT_EQ(indHigh, std::size_t(9));
	ASSERT_EQ(mode, NA);
}

TEST_F(FieldsTest, SortFieldsTest) {
	Fields fields;
	fields.add("Field1", 19, 30, "", RW);
	fields.add("Field2", 12, 18, "", RW);
	fields.add("Field3", 54, 82, "", RW);
	fields.add("Field4", 0, 4, "", RW);
	fields.add("Field5", 10, 11, "", RW);
	fields.add("Field6", 5, 9, "", RW);

	std::vector<std::pair<std::string, Field>> tmp = fields.getFieldsSortedByIndex();
	ASSERT_EQ(tmp[0].second.getIndLow(), 0u);
	ASSERT_EQ(tmp[1].second.getIndLow(), 5u);
	ASSERT_EQ(tmp[2].second.getIndLow(), 10u);
	ASSERT_EQ(tmp[3].second.getIndLow(), 12u);
	ASSERT_EQ(tmp[4].second.getIndLow(), 19u);
	ASSERT_EQ(tmp[5].second.getIndLow(), 54u);
}
