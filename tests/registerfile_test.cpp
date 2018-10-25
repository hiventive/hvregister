/**
 * @file registerfile_test.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Mar, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Tests for registerfile.h
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include <hv/common.h>

#include "registerfile/registerfile.h"

using namespace ::hv::common;
using namespace ::hv::reg;

class RegisterFileTest: public ::testing::Test {
	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};

TEST_F(RegisterFileTest, GeneralTest) {
	const std::size_t regSize(32);
	// Creating registers
	Register r1(regSize, "Register1", "This is the first register.", RW);
	Register r2(regSize, "Register2", "This is the second register.", RO);
	Register r3(regSize, "Register3", "This is the third register.", WO);

	// Adding fields
	r1.createField("Field_1_1", 2, 5, "This is the first field of r1");
	r1.createField("Field_1_2", 20, 27, "This is the second field of r1");
	r2.createField("Field_2_1", 12, 19, "This is the first field of r2");
	r2.createField("Field_2_2", 24, 27, "This is the second field of r2");
	r3.createField("Field_3_1", 1, 13, "This is the first field of r3");
	r3.createField("Field_3_2", 15, 24, "This is the second field of r3");

	// Creating register file with a 32-bit alignment (4 bytes)
	RegisterFile rb("RegFile", "This is register file description.", 4);

	// Adding external registers
	ASSERT_TRUE(rb.addRegister(0x0, r1))<< "Could not insert r1";
	ASSERT_TRUE(rb.addRegister(0xF4, r2))<< "Could not insert r2";
	ASSERT_TRUE(rb.addRegister(0x23C, r3))<< "Could not insert r3";

	// Adding internal registers
	ASSERT_TRUE(rb.createRegister(0x14C8, regSize, "Register4", "This is the fourth register.", RO))<< "Could not create/insert r4";
	ASSERT_TRUE(rb.createRegister(0x378C, regSize, "Register5", "This is the fifth register.", RO))<< "Could not create/insert r5";

	// Retrieving references to internal registers
	Register &r4 = rb.getRegister("Register4");
	Register&r5 = rb.getRegister(0x378C);

	// Adding fields to r4 and r5
	r4.createField("Field_4_1", 6, 12, "This is the first field of r4");
	r4.createField("Field_4_2", 25, 30, "This is the second field of r4");
	r5.createField("Field_5_1", 2, 13, "This is the first field of r5");
	r5.createField("Field_5_2", 21, 28, "This is the second field of r5");

	// Setting values to all registers
	r1 = 0xFFFFFFFF;
	r2 = 0xF0F0F0F0;
	r3 = 0x0F0F0F0F;
	r4 = 0xAAAAAAAA;
	r5 = 0x55555555;

	// Checking values
	ASSERT_EQ(hvuint32_t(rb.getRegister("Register1")), hvuint32_t(0xFFFFFFFF));
	ASSERT_EQ(hvuint32_t(rb.getRegister("Register2")), hvuint32_t(0xF0F0F0F0));
	ASSERT_EQ(hvuint32_t(rb.getRegister("Register3")), hvuint32_t(0x0F0F0F0F));
	ASSERT_EQ(hvuint32_t(rb.getRegister("Register4")), hvuint32_t(0xAAAAAAAA));
	ASSERT_EQ(hvuint32_t(rb.getRegister("Register5")), hvuint32_t(0x55555555));

	std::cout << rb.getInfo() << std::endl;
}

TEST_F(RegisterFileTest, RegFileInRegFileTest) {
	const std::size_t regSize(32);
	// Creating registers
	Register r1(regSize, "Register1", "This is the first register", RW);
	Register r2(regSize, "Register2", "This is the second register", RO);
	Register r3(regSize, "Register3", "This is the third register", WO);

	// Adding fields
	r1.createField("Field_1_1", 2, 5, "This is the first field of r1");
	r1.createField("Field_1_2", 20, 27, "This is the second field of r1");
	r2.createField("Field_2_1", 12, 19, "This is the first field of r2");
	r2.createField("Field_2_2", 24, 27, "This is the second field of r2");
	r3.createField("Field_3_1", 1, 13, "This is the first field of r3");
	r3.createField("Field_3_2", 15, 24, "This is the second field of r3");

	// Creating register file
	RegisterFile rb("MainRegisterFile", "This is the main register file.", 4);

	// Adding external registers (addresses in decimal for easier testing)
	ASSERT_TRUE(rb.addRegister(0x0, r1))<< "Could not insert r1";
	ASSERT_TRUE(rb.addRegister(0x1000, r2))<< "Could not insert r2";
	ASSERT_TRUE(rb.addRegister(0x2000, r3))<< "Could not insert r3";

	// Creating register file
	RegisterFile rbIn("SubRegisterFile1", "This is the first sub-register file",
			4);
	ASSERT_TRUE(rbIn.createRegister(0x0, regSize, "SubRegister1_0", "This is the zeroth internal register (1)", RO))<< "Could not insert register in registerFile";
	ASSERT_TRUE(rbIn.createRegister(0x42C, regSize,"SubRegister1_2", "This is the second internal register (1)", RO))<< "Could not insert register in registerFile";

	Register rY(regSize, "SubRegister1_1", "This is the first internal register (1)",
			RO);
	rbIn.addRegister(0x130, rY);
	ASSERT_TRUE(rb.addRegisterFile(0x8, rbIn));

	ASSERT_TRUE(
			rb.createRegister(0x4000, regSize, "Register4",
					"This the fourth register.", WO));
	RegisterFile rbIn2("SubRegisterFile2",
			"This is the second sub-register file", 4);
	ASSERT_TRUE(
			rbIn2.createRegister(0x2B4, regSize, "SubRegister2_1",
					"This is the first internal register (2)", WO));
	ASSERT_TRUE(
			rbIn2.createRegister(0x4CC, regSize, "SubRegister2_2",
					"This is the second internal register (2)", RW));
	ASSERT_TRUE(rb.addRegisterFile(0x4090, rbIn2, 0x500));
	std::cout << rb.getRecursiveInfo() << std::endl;
}

TEST_F(RegisterFileTest, CloneTest) {
	const std::size_t regSize(128);
	RegisterFile rf("regFile", "This is a regFile");
	rf.createRegister(regSize, "Register 1", "This is register 1", RW);
	rf.createRegister(regSize, "Register 2", "This is register 2", RW);
	rf.createRegister(regSize, "Register 3", "This is register 3", RW);
	rf.createRegister(regSize, "Register 4", "This is register 4", RW);

	RegisterFile rf2(rf);

	ASSERT_STREQ(rf.getName().c_str(), rf2.getName().c_str());
	ASSERT_STREQ(rf.getDescription().c_str(), rf2.getDescription().c_str());
	ASSERT_STREQ(rf.getRegister("Register 1").getName().c_str(),
			rf2.getRegister("Register 1").getName().c_str());
	ASSERT_STREQ(rf.getRegister("Register 2").getName().c_str(),
			rf2.getRegister("Register 2").getName().c_str());
	ASSERT_STREQ(rf.getRegister("Register 3").getName().c_str(),
			rf2.getRegister("Register 3").getName().c_str());
	ASSERT_STREQ(rf.getRegister("Register 4").getName().c_str(),
			rf2.getRegister("Register 4").getName().c_str());

	ASSERT_STREQ(rf.getRegister("Register 1").getDescription().c_str(),
			rf2.getRegister("Register 1").getDescription().c_str());
	ASSERT_STREQ(rf.getRegister("Register 2").getDescription().c_str(),
			rf2.getRegister("Register 2").getDescription().c_str());
	ASSERT_STREQ(rf.getRegister("Register 3").getDescription().c_str(),
			rf2.getRegister("Register 3").getDescription().c_str());
	ASSERT_STREQ(rf.getRegister("Register 4").getDescription().c_str(),
			rf2.getRegister("Register 4").getDescription().c_str());

}

TEST_F(RegisterFileTest, UnaddressedTest) {
	const std::size_t regSize(24);
	// Creating registers
	Register r1(regSize, "Register1", "This is the first register", RW);
	Register r2(regSize, "Register2", "This is the second register", RO);
	Register r3(regSize, "Register3", "This is the third register", WO);

	RegisterFile rb("MainRegisterFile", "This is the main register file.", 16);

	rb.addRegister(r1);
	rb.addRegister(r2);
	rb.addRegister(r3);

	RegisterFile rbIn("InternalRegisterFile",
			"This is the first internal register file.", 16);

	Register r4(regSize, "Register4", "This is the fourth register", RW);
	Register r5(regSize, "Register5", "This is the fifth register", RW);
	rbIn.addRegister(r4);
	rbIn.addRegister(r5);

	rb.addRegisterFile(rbIn);

	Register r6(regSize, "Register6", "This is the sixth register", RW);
	rb.addRegister(r6);

	RegisterFile rbIn2("InternalRegisterFile2",
			"This is the second internal register file.", 16);

	Register r7(regSize, "Register7", "This is the seventh register", WO);
	rbIn2.addRegister(r7);

	rb.addRegisterFile(rbIn2);

	std::cout << rb.getRecursiveInfo() << std::endl;

}

TEST_F(RegisterFileTest, RegisterBlockCreationTest) {
	std::size_t regSize(32);
	RegisterFile topRF("RegFile", "This is a reg file", 4);
	topRF.createRegisterBlock(0x4, 12, regSize, "Reg", "This is a reg", RW);

//	std::cout << topRF.getInfo() << std::endl;

	ASSERT_EQ(topRF.getRegisterAddress("Reg_0"), hvaddr_t(0x4));
	ASSERT_EQ(topRF.getRegisterAddress("Reg_1"), hvaddr_t(0x8));
	ASSERT_EQ(topRF.getRegisterAddress("Reg_2"), hvaddr_t(0xC));
	ASSERT_EQ(topRF.getRegisterAddress("Reg_3"), hvaddr_t(0x10));
	ASSERT_EQ(topRF.getRegisterAddress("Reg_4"), hvaddr_t(0x14));
}

//TEST(RegisterFileTest, StartingUpGuideTest) {
//	// Top register file creation
//	RegisterFile topRF("Top Register File", "This is the top register file", 4);
//
//	// Registers creation
//	Register<32> r1("Register 1", "This is register 1 of topRF.", READ);
//	Register<32> r2("Register 2", "This is register 2 of topRF.", WRITE);
//	Register<24> r3("Register 3", "This is register 3 of topRF.", READ_WRITE);
//
//	// Adding registers to top register file
//	topRF.addRegister(r1);
//	topRF.addRegister(r2);
//	topRF.addRegister(r3);
//
//	// Displaying top register file information
//	std::cout << topRF.getInfo() << std::endl;
//
//	// Modifying r1
//	std::cout << "Before - (r1):\t\t\t" << r1 << std::endl;
//	std::cout << "Before - (r1 through topRF):\t" << topRF.getRegister < 32
//			> ("Register 1") << std::endl;
//	r1 = 0x0F0F0F0F;
//	std::cout << "After - (r1):\t\t\t" << r1 << std::endl;
//	std::cout << "After - (r1 through topRF):\t" << topRF.getRegister < 32
//			> ("Register 1") << std::endl;
//
//	// Register creation and add by copy
//	Register<64> r4("Register 4", "This is register 4 of topRF.", READ_WRITE);
//	topRF.addRegisterCopy(0x10, r4);
//
//	std::cout << "Before - (r4):\t\t\t" << r4 << std::endl;
//	std::cout << "Before - (r4 through topRF):\t" << topRF.getRegister < 64
//			> (0x10) << std::endl;
//
//	r4 = 0x5555555555555555;
//
//	std::cout << "After - (r4):\t\t\t" << r4 << std::endl;
//	std::cout << "After - (r4 through topRF):\t" << topRF.getRegister < 64
//			> (0x10) << std::endl;
//
//	std::cout << topRF.getInfo() << std::endl;
//
//	topRF.createRegister < 32
//			> (0x100, "Register 5", "This is register 5 of topRF.", READ);
//
//	std::cout << topRF.getInfo() << std::endl;
//
//	// Creating internal register file
//	RegisterFile intRF1("InternalRegisterFile1",
//			"This is an internal reg. file.", 4);
//
//	// Creating registers inside intRF1
//	intRF1.createRegister < 16
//			> ("IntRF1 Register 1", "This is register 1 of intRF1.", WRITE);
//	intRF1.createRegister < 32
//			> (0xC, "IntRF1 Register 2", "This is register 2 of intRF1.", READ_WRITE);
//	intRF1.createRegister < 32
//			> ("IntRF1 Register 3", "This is register 3 of intRF1.", READ_WRITE);
//
//	std::cout << intRF1.getInfo() << std::endl;
//
//	// Adding "Register 4" from topRF to intRF1
//	intRF1.addRegister(0x24, topRF.getRegister < 24 > ("Register 4"));
//
//	std::cout << intRF1.getInfo() << std::endl;
//
//	intRF1.getRegister < 24 > (0x24) = 0xFFFFFF;
//
//	std::cout << "Register 4 from topRF:  " << topRF.getRegister < 24
//			> ("Register 4") << std::endl;
//	std::cout << "Register 4 from intRF1: " << topRF.getRegister < 24
//			> ("Register 4") << std::endl;
//
//	// Inserting intRF1 into topRF
//	topRF.addRegisterFile(intRF1);
//
//	std::cout << topRF.getInfo() << std::endl;
//
//	// Creating intRF2
//	RegisterFile intRF2("InternalRegisterFile2",
//			"This is an internal reg. file.", 4);
//	intRF2.createRegister < 32
//			> (0xC, "IntRF2 Register 1", "This is register 1 of intRF2", READ_WRITE);
//	intRF2.createRegister < 32
//			> ("IntRF2 Register 2", "This is register 2 of intRF2", READ_WRITE);
//	intRF2.createRegister < 32
//			> ("IntRF2 Register 3", "This is register 3 of intRF2", READ_WRITE);
//	intRF2.createRegister < 32
//			> ("IntRF2 Register 4", "This is register 4 of intRF2", READ_WRITE);
//
//	std::cout << intRF2.getInfo() << std::endl;
//
//	topRF.addRegisterFile(0x20, intRF2, 36);
//
//	std::cout << topRF.getInfo() << std::endl;
//
//	std::cout << std::endl;
//	std::cout << std::endl;
//	std::cout << std::endl;
//	std::cout << std::endl;
//
//	std::cout << topRF.getRecursiveInfo() << std::endl;
//
//	std::cout << std::endl;
//	std::cout << std::endl;
//	std::cout << std::endl;
//	std::cout << std::endl;
//
//	std::cout << topRF.getFlattenedInfo() << std::endl;
//}
