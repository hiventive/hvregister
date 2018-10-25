/**
 * @file register_test.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register test
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include <hv/common.h>

#include "register/register-private.h"

using namespace ::hv::common;
using namespace ::hv::reg;

class RegisterTest: public ::testing::Test {
protected:
	virtual void SetUp() {
		nTests = 1000;
	}

	virtual void TearDown() {
	}

	hvuint32_t nTests;
};

TEST_F(RegisterTest, MemberAccessTest) {
	Register r1(32, "Register1", "This is Register1", RW, 0xFF00FF00);
	ASSERT_STREQ(r1.getName().c_str(), std::string("Register1").c_str());
	ASSERT_STREQ(r1.getDescription().c_str(),
			std::string("This is Register1").c_str());
	ASSERT_EQ(r1.getSize(), std::size_t(32));
	ASSERT_EQ(r1.getRWMode(), RW);
	ASSERT_EQ(hvuint32_t(r1.getValue()), hvuint32_t(0xFF00FF00));
	ASSERT_EQ(hvuint32_t(r1.getReadMask()), ~hvuint32_t(0u));
	ASSERT_EQ(hvuint32_t(r1.getWriteMask()), ~hvuint32_t(0u));

	Register r2(16, "Register2", "This is Register2", RO, 0x0F0F);
	ASSERT_STREQ(r2.getName().c_str(), std::string("Register2").c_str());
	ASSERT_STREQ(r2.getDescription().c_str(),
			std::string("This is Register2").c_str());
	ASSERT_EQ(r2.getSize(), std::size_t(16));
	ASSERT_EQ(r2.getRWMode(), RO);
	ASSERT_EQ(hvuint16_t(r2.getValue()), hvuint16_t(0x0F0F));
	ASSERT_EQ(hvuint16_t(r2.getReadMask()), hvuint16_t(0xFFFF));
	ASSERT_EQ(hvuint16_t(r2.getWriteMask()), hvuint16_t(0x0000));

	Register r3(72, "Register3", "This is Register3", WO);
	ASSERT_STREQ(r3.getName().c_str(), std::string("Register3").c_str());
	ASSERT_STREQ(r3.getDescription().c_str(),
			std::string("This is Register3").c_str());
	ASSERT_EQ(r3.getSize(), std::size_t(72));
	ASSERT_EQ(r3.getRWMode(), WO);
	ASSERT_EQ(hvuint32_t(r3.getValue()), hvuint32_t(0x00000000));
	ASSERT_EQ(hvuint32_t(r3.getReadMask()), hvuint32_t(0x00000000));
	ASSERT_EQ(hvuint32_t(r3.getWriteMask()), hvuint32_t(0xFFFFFFFF));
}

TEST_F(RegisterTest, AssignmentTest) {
	Register r1(32, "Register1", "This is Register1", RW, 0x0);
	ASSERT_EQ(hvuint32_t(r1.getValue()), hvuint32_t(0u));
	for (hvuint32_t i = 0; i < nTests; i++) {
		hvuint8_t val, valRet;
		val = ::hv::common::test::randNumGen<hvuint8_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvuint8_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvuint16_t val, valRet;
		val = ::hv::common::test::randNumGen<hvuint16_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvuint16_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvuint32_t val, valRet;
		val = ::hv::common::test::randNumGen<hvuint32_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvuint32_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvuint64_t val, valRet;
		val = ::hv::common::test::randNumGen<hvuint64_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvuint64_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvint8_t val, valRet;
		val = ::hv::common::test::randNumGen<hvint8_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvint8_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvint16_t val, valRet;
		val = ::hv::common::test::randNumGen<hvint16_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvint16_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvint32_t val, valRet;
		val = ::hv::common::test::randNumGen<hvint32_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvint32_t failed";
	}

	for (hvuint32_t i = 0; i < nTests; i++) {
		hvint64_t val, valRet;
		val = ::hv::common::test::randNumGen<hvint64_t>(32);
		r1 = val;
		valRet = r1;
		ASSERT_EQ(val, valRet)<< "Assignment/cast from hvint64_t failed";
	}
}

TEST_F(RegisterTest, ReadWriteMasksTest) {
	Register r1(64, "myRegister1", "This is my register and only mine", RW);
	ASSERT_EQ(hvuint64_t(r1.getReadMask()), hvuint64_t(0xFFFFFFFFFFFFFFFF));
	ASSERT_EQ(hvuint64_t(r1.getWriteMask()), hvuint64_t(0xFFFFFFFFFFFFFFFF));

	Register r2(32, "myRegister2", "This is my register and only mine", RO);
	ASSERT_EQ(hvuint32_t(r2.getReadMask()), hvuint32_t(0xFFFFFFFF));
	ASSERT_EQ(hvuint32_t(r2.getWriteMask()), hvuint32_t(0x00000000));

	Register r3(16, "myRegister3", "This is my register and only mine", WO);
	ASSERT_EQ(hvuint16_t(r3.getReadMask()), hvuint16_t(0x0000));
	ASSERT_EQ(hvuint16_t(r3.getWriteMask()), hvuint16_t(0xFFFF));

	Register r4(32, "myRegister4", "This is my register and only mine");
	ASSERT_EQ(hvuint32_t(r4.getReadMask()), hvuint32_t(0xFFFFFFFF));
	ASSERT_EQ(hvuint32_t(r4.getWriteMask()), hvuint32_t(0xFFFFFFFF));

	r4.createField("field1", 3, 0, RO);
	ASSERT_EQ(hvuint32_t(r4.getReadMask()), hvuint32_t(0xFFFFFFFF));
	ASSERT_EQ(hvuint32_t(r4.getWriteMask()), hvuint32_t(0xFFFFFFF0));

	r4.createField("field2", 4, WO);
	ASSERT_EQ(hvuint32_t(r4.getReadMask()), hvuint32_t(0xFFFFFFEF));
	ASSERT_EQ(hvuint32_t(r4.getWriteMask()), hvuint32_t(0xFFFFFFF0));

	r4.createField("field3", 5, 15, "Some Field", RW);
	ASSERT_EQ(hvuint32_t(r4.getReadMask()), hvuint32_t(0xFFFFFFEF));
	ASSERT_EQ(hvuint32_t(r4.getWriteMask()), hvuint32_t(0xFFFFFFF0));

	r4.createField("field4", 31, 16, RO);
	ASSERT_EQ(hvuint32_t(r4.getReadMask()), hvuint32_t(0xFFFFFFEF));
	ASSERT_EQ(hvuint32_t(r4.getWriteMask()), hvuint32_t(0x0000FFF0));

	r4.setReadMask(0x0F0F0F0F);
	ASSERT_EQ(hvuint32_t(r4.getReadMask()), hvuint32_t(0x0F0F0F0F));
	r4.setWriteMask(0xF0F0F0F0);
	ASSERT_EQ(hvuint32_t(r4.getWriteMask()), hvuint32_t(0xF0F0F0F0));
}

TEST_F(RegisterTest, FieldManipulationTest) {
	Register r(32, "Register", "Great Register", RW, 0);
	r.createField("Field1", 7, 0);
	r.createField("Field2", 15, 8);
	r.createField("Field3", 23, 16);
	r.createField("Field4", 31, 24);

	ASSERT_EQ(hvuint32_t(r), hvuint32_t(0x00000000));
	ASSERT_EQ(hvuint32_t(r("Field1")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field2")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field3")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field4")), hvuint32_t(0x00));

	r("Field1") = 0xFF;
	ASSERT_EQ(hvuint32_t(r), hvuint32_t(0x000000FF));
	ASSERT_EQ(hvuint32_t(r("Field1")), hvuint32_t(0xFF));
	ASSERT_EQ(hvuint32_t(r("Field2")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field3")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field4")), hvuint32_t(0x00));

	r("Field3") = 0x55;
	ASSERT_EQ(hvuint32_t(r), hvuint32_t(0x005500FF));
	ASSERT_EQ(hvuint32_t(r("Field1")), hvuint32_t(0xFF));
	ASSERT_EQ(hvuint32_t(r("Field2")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field3")), hvuint32_t(0x55));
	ASSERT_EQ(hvuint32_t(r("Field4")), hvuint32_t(0x00));

	r("Field4") = r("Field1");
	ASSERT_EQ(hvuint32_t(r), hvuint32_t(0xFF5500FF));
	ASSERT_EQ(hvuint32_t(r("Field1")), hvuint32_t(0xFF));
	ASSERT_EQ(hvuint32_t(r("Field2")), hvuint32_t(0x00));
	ASSERT_EQ(hvuint32_t(r("Field3")), hvuint32_t(0x55));
	ASSERT_EQ(hvuint32_t(r("Field4")), hvuint32_t(0xFF));

	r("Field2") = r("Field4") ^ r("Field3");
	ASSERT_EQ(hvuint32_t(r), hvuint32_t(0xFF55AAFF));
	ASSERT_EQ(hvuint32_t(r("Field1")), hvuint32_t(0xFF));
	ASSERT_EQ(hvuint32_t(r("Field2")), hvuint32_t(0xAA));
	ASSERT_EQ(hvuint32_t(r("Field3")), hvuint32_t(0x55));
	ASSERT_EQ(hvuint32_t(r("Field4")), hvuint32_t(0xFF));

	r("Field2") |= (r("Field2") >> 1u);
	ASSERT_EQ(hvuint32_t(r), hvuint32_t(0xFF55FFFF));
	ASSERT_EQ(hvuint32_t(r("Field1")), hvuint32_t(0xFF));
	ASSERT_EQ(hvuint32_t(r("Field2")), hvuint32_t(0xFF));
	ASSERT_EQ(hvuint32_t(r("Field3")), hvuint32_t(0x55));
	ASSERT_EQ(hvuint32_t(r("Field4")), hvuint32_t(0xFF));

}

TEST_F(RegisterTest, DisplayTest) {
	Register r(32, "Register", "Great Register", RW, 0);
	r.createField("Field1", 7, 0);
	r.createField("Field2", 15, 8);
	r.createField("Field3", 23, 16);
	r.createField("Field4", 31, 24);

	r("Field3") = 0xFF;

	std::cout << r.getInfo(0) << std::endl;

	Register r2(32, "Register", "Great Register", RW, 0);
	r2.createField("Field1", 7, 3);
	r2.createField("Field2", 12, 9);
	r2.createField("Field3", 20, 15);
	r2.createField("Field4", 28, 24);

	r2("Field1") = 0x1F;
	r2("Field3") = 0xFF;

	std::cout << r2.getInfo() << std::endl;
}

TEST_F(RegisterTest, ReadWriteTest) {
	hvuint8_t readBuff[4];
	hvuint8_t writeBuff[4];
	std::size_t writeSize(4);
	Register r(32, "Reg", "Reg. Description.", RW);
	for (unsigned int i = 0; i < nTests; i++) {
		// Filling writeBuff
		for (std::size_t j = 0; j < 4; j++) {
			writeBuff[j] = static_cast<hvuint8_t>(rand() % 256);
		}
		ASSERT_TRUE(r.write(writeBuff,writeSize))<< "Error in write";
		ASSERT_TRUE(r.read(readBuff,writeSize))<< "Error in read";
		for (std::size_t j = 0; j < writeSize; j++) {
			ASSERT_EQ(readBuff[j], writeBuff[j])<< "Problem in read/write sequence (cell " << j << ")";
		}
	}
}

// Callback registration test
enum funcCalled {
	PRE_READ_CLASS_METHOD,
	POST_READ_CLASS_METHOD,
	PRE_WRITE_CLASS_METHOD,
	POST_WRITE_CLASS_METHOD,
	PRE_READ_FUNCTION,
	POST_READ_FUNCTION,
	PRE_WRITE_FUNCTION,
	POST_WRITE_FUNCTION,
	PRE_READ_LAMBDA,
	POST_READ_LAMBDA,
	PRE_WRITE_LAMBDA,
	POST_WRITE_LAMBDA,
};

std::vector<funcCalled> whoWasCalled;

class readFoo {
public:
	readFoo() :
			val(0) {
	}

	BitVector getVal() {
		return val;
	}

	bool myPreFunc(const ::hv::reg::RegisterReadEvent &ev) {
		whoWasCalled.push_back(funcCalled::PRE_READ_CLASS_METHOD);
		this->val = ev.value;
		return true;
	}

	void myPostFunc(const ::hv::reg::RegisterReadEvent &ev) {
		whoWasCalled.push_back(funcCalled::POST_READ_CLASS_METHOD);
		this->val = ev.value;
	}

protected:
	BitVector val;
};

class writeFoo {
public:
	writeFoo() :
			oldVal(0), newVal(0) {
	}

	BitVector getOldVal() {
		return oldVal;
	}

	BitVector getNewVal() {
		return newVal;
	}

	bool myPreFunc(const ::hv::reg::RegisterWriteEvent &ev) {
		whoWasCalled.push_back(funcCalled::PRE_WRITE_CLASS_METHOD);
		this->oldVal = ev.oldValue;
		this->newVal = ev.newValue;
		return true;
	}

	void myPostFunc(const ::hv::reg::RegisterWriteEvent &ev) {
		whoWasCalled.push_back(funcCalled::POST_WRITE_CLASS_METHOD);
		this->oldVal = ev.oldValue;
		this->newVal = ev.newValue;
	}

protected:
	BitVector oldVal;
	BitVector newVal;
};

bool barPreReadFunc(const ::hv::reg::RegisterReadEvent &ev) {
	whoWasCalled.push_back(funcCalled::PRE_READ_FUNCTION);
	return true;
}

void barPostReadFunc(const ::hv::reg::RegisterReadEvent &ev) {
	whoWasCalled.push_back(funcCalled::POST_READ_FUNCTION);
}

bool barPreWriteFunc(const ::hv::reg::RegisterWriteEvent &ev) {
	whoWasCalled.push_back(funcCalled::PRE_WRITE_FUNCTION);
	return true;
}

void barPostWriteFunc(const ::hv::reg::RegisterWriteEvent &ev) {
	whoWasCalled.push_back(funcCalled::POST_WRITE_FUNCTION);
}

TEST_F(RegisterTest, CallbackRegistrationUnregistrationTest) {
	// Register creation
	Register reg(32, "Register name", "Register description", RW);

	// Registering read callbacks
	typedef readFoo readfoo_t;
	readfoo_t rf;
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(0));
	hvuint32_t preReadCBId0 = reg.registerPreReadCallback(
			std::bind(&readfoo_t::myPreFunc, &rf, std::placeholders::_1));
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(1));
	hvuint32_t preReadCBId1 = reg.registerPreReadCallback(barPreReadFunc);
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(2));
	hvuint32_t preReadCBId2 =
			reg.registerPreReadCallback(
					[](const ::hv::reg::RegisterReadEvent&) {whoWasCalled.push_back(funcCalled::PRE_READ_LAMBDA); return true;});
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(3));

	ASSERT_EQ(reg.howManyPostReadCallbacks(), std::size_t(0));
	reg.registerPostReadCallback(
			std::bind(&readfoo_t::myPostFunc, &rf, std::placeholders::_1));
	ASSERT_EQ(reg.howManyPostReadCallbacks(), std::size_t(1));
	reg.registerPostReadCallback(barPostReadFunc);
	ASSERT_EQ(reg.howManyPostReadCallbacks(), std::size_t(2));
	reg.registerPostReadCallback(
			[](const ::hv::reg::RegisterReadEvent&) {whoWasCalled.push_back(funcCalled::POST_READ_LAMBDA);});
	ASSERT_EQ(reg.howManyPostReadCallbacks(), std::size_t(3));

	// Registering write callbacks
	typedef writeFoo writefoo_t;
	writefoo_t wf;
	ASSERT_EQ(reg.howManyPreWriteCallbacks(), std::size_t(0));
	reg.registerPreWriteCallback(
			std::bind(&writefoo_t::myPreFunc, &wf, std::placeholders::_1));
	ASSERT_EQ(reg.howManyPreWriteCallbacks(), std::size_t(1));
	reg.registerPreWriteCallback(barPreWriteFunc);
	ASSERT_EQ(reg.howManyPreWriteCallbacks(), std::size_t(2));
	reg.registerPreWriteCallback(
			[](const ::hv::reg::RegisterWriteEvent&) {whoWasCalled.push_back(funcCalled::PRE_WRITE_LAMBDA); return true;});
	ASSERT_EQ(reg.howManyPreWriteCallbacks(), std::size_t(3));

	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(0));
	hvuint32_t postWriteCBId0 = reg.registerPostWriteCallback(
			std::bind(&writefoo_t::myPostFunc, &wf, std::placeholders::_1));
	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(1));
	hvuint32_t postWriteCBId1 = reg.registerPostWriteCallback(barPostWriteFunc);
	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(2));
	hvuint32_t postWriteCBId2 =
			reg.registerPostWriteCallback(
					[](const ::hv::reg::RegisterWriteEvent&) {whoWasCalled.push_back(funcCalled::POST_WRITE_LAMBDA);});
	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(3));

	// Writing value
	const std::size_t SIZE = 32 / 8;
	hvuint8_t writeBuff[SIZE];
	for (std::size_t i = 0; i < SIZE; i++) {
		writeBuff[i] = 0x55; //01010101
	}
	bool retWrite = reg.write(writeBuff, SIZE);
	ASSERT_TRUE(retWrite)<< "Error while writing in register";

	// Reading value
	hvuint8_t readBuff[SIZE];
	bool retRead = reg.read(readBuff, SIZE);
	ASSERT_TRUE(retRead)<< "Error while reading from register";
	for (std::size_t i = 0; i < SIZE; i++) {
		ASSERT_EQ(readBuff[i], hvuint8_t(0x55))<< "Error in read data (buff position " << i << ")";
	}

	// Checking callbacks stats
	ASSERT_EQ(whoWasCalled.size(), std::size_t(12))<< "Error in the number of callbacks called";

	ASSERT_EQ(whoWasCalled[0],funcCalled::PRE_WRITE_CLASS_METHOD)<< "Error in 1st pre-write";
	ASSERT_EQ(whoWasCalled[1],funcCalled::PRE_WRITE_FUNCTION)<< "Error in 2nd pre-write";
	ASSERT_EQ(whoWasCalled[2],funcCalled::PRE_WRITE_LAMBDA)<< "Error in 3rd pre-write";

	ASSERT_EQ(whoWasCalled[3],funcCalled::POST_WRITE_CLASS_METHOD)<< "Error in 1st post-write";
	ASSERT_EQ(whoWasCalled[4],funcCalled::POST_WRITE_FUNCTION)<< "Error in 2nd post-write";
	ASSERT_EQ(whoWasCalled[5],funcCalled::POST_WRITE_LAMBDA)<< "Error in 3rd post-write";

	ASSERT_EQ(whoWasCalled[6],funcCalled::PRE_READ_CLASS_METHOD)<< "Error in 1st pre-read";
	ASSERT_EQ(whoWasCalled[7],funcCalled::PRE_READ_FUNCTION)<< "Error in 2nd pre-read";
	ASSERT_EQ(whoWasCalled[8],funcCalled::PRE_READ_LAMBDA)<< "Error in 3rd pre-Read";

	ASSERT_EQ(whoWasCalled[9],funcCalled::POST_READ_CLASS_METHOD)<< "Error in 1st post-read";
	ASSERT_EQ(whoWasCalled[10],funcCalled::POST_READ_FUNCTION)<< "Error in 2nd post-read";
	ASSERT_EQ(whoWasCalled[11],funcCalled::POST_READ_LAMBDA)<< "Error in 3rd post-read";

	// Unregistering callbacks
	ASSERT_EQ(reg.howManyCallbacks(), 12u);
	reg.unregisterPreReadCallback(preReadCBId2);
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(2));

	reg.unregisterPreReadCallback(preReadCBId0);
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(1));

	reg.unregisterPreReadCallback(preReadCBId1);
	ASSERT_EQ(reg.howManyPreReadCallbacks(), std::size_t(0));

	reg.unregisterPostWriteCallback(postWriteCBId2);
	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(2));

	reg.unregisterPostWriteCallback(postWriteCBId0);
	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(1));

	reg.unregisterPostWriteCallback(postWriteCBId1);
	ASSERT_EQ(reg.howManyPostWriteCallbacks(), std::size_t(0));

	ASSERT_EQ(reg.howManyCallbacks(), 6u);

	reg.unregisterAllCallbacks();
	ASSERT_EQ(reg.howManyCallbacks(), 0u);

}

// Some function we can register as post-read callback
void displayValuesAfterWriting(const ::hv::reg::RegisterWriteEvent &ev) {
	std::cout << ev.rh.getName() << " - Write event old value: " << ev.oldValue
			<< std::endl;
	std::cout << ev.rh.getName() << " - Write event new value: " << ev.newValue
			<< std::endl << std::endl;
}

TEST_F(RegisterTest, StartingGuideTest) {
	// Register declaration
	Register reg(32, "MyRegister", "This is my register.", RW, 0);
	std::cout << reg.getInfo() << std::endl;

	// Creating field
	reg.createField("Field1", 7, 1, "My first field.");
	std::cout << reg.getInfo() << std::endl;

	// Creating more fields
	reg.createField("Field2", 9, "My second field.");
	reg.createField("Field3", 31, 20);
	std::cout << reg.getInfo() << std::endl;

	// Register declaration
	Register reg2(16, "myNewRegister", "This is my new register", RW);

	// Fields creation
	reg2.createField("f1", 3, 0);
	reg2.createField("f2", 7, 4);
	reg2.createField("f3", 11, 8);
	reg2.createField("f4", 15, 12);

	std::cout << reg2.getInfo(0) << std::endl;

	// Modifying register value
	reg2 = 0x0F0F;
	std::cout << reg2.getInfo(0) << std::endl;

	// Modifying f1's value
	reg2("f1") = 5;
	std::cout << reg2.getInfo(0) << std::endl;

	// Setting f4's value with f1's
	reg2("f4") = reg2("f1");
	std::cout << reg2.getInfo(0) << std::endl;

	// Doing whatever you want
	reg2("f2")(3, 1) = (reg2("f1") | ~reg2("f4"))(2, 0);
	std::cout << reg2.getInfo(0) << std::endl;

	// Interoperability example
	hvuint32_t x = (reg2(15, 8) & 0xF0) + reg2("f1");
	std::cout << std::hex << "x = 0x" << x << std::endl;

	//** Callback registration example **//

	struct fooStruct {
		// Some register
		Register reg;

		// Some class method we can register as pre-read callback
		bool checkSomethingBeforeReading(
				const ::hv::reg::RegisterReadEvent &ev) const {
			std::cout << ev.rh.getName() << " - Read event value: " << ev.value
					<< std::endl;
			std::cout << reg.getName() << " - My current value: " << reg
					<< std::endl << std::endl;
			// Returning true to allow reading
			return true;
		}

		// fooStruct constructor
		fooStruct(const std::string &name) :
				reg(32, name, "", RW) {
		}

	};

	// Instantiating fooStructs
	fooStruct myFooInstance("myReg"), myOtherFooInstance("myOtherReg");

	// Directly modifying myFooInstance.reg value
	myFooInstance.reg = 0x55555555;
	// Directly modifying myOtherFooInstance.reg value
	myOtherFooInstance.reg = 0x33333333;

	// Registering checkSomethingBeforeReading(.) to myFooInstance.reg associated to myFooInstance.reg
	myFooInstance.reg.registerPreReadCallback(&fooStruct::checkSomethingBeforeReading, &myFooInstance);

	// Registering displayValuesAfterWriting(.) to myFooInstance.reg
	myFooInstance.reg.registerPostWriteCallback(displayValuesAfterWriting);

	// Registering checkSomethingBeforeReading(.) to myOtherFooInstance.reg associated to myFooInstance.reg
	myOtherFooInstance.reg.registerPreReadCallback(
			std::bind(&fooStruct::checkSomethingBeforeReading, &myFooInstance,
					std::placeholders::_1));

	// Registering displayValuesAfterWriting(.) to myOtherFooInstance.reg
	myOtherFooInstance.reg.registerPostWriteCallback(displayValuesAfterWriting);

	// 32-bit buffer for reading and writing
	hvuint8_t* buffer = (hvuint8_t*) malloc(4 * sizeof(hvuint8_t));

	std::cout << "myFooInstance.reg = " << myFooInstance.reg << std::endl;
	std::cout << "myFooOtherInstance.reg = " << myOtherFooInstance.reg
			<< std::endl;

	// Filling buffer
	buffer[0] = 0xF0;
	buffer[1] = 0xF0;
	buffer[2] = 0xF0;
	buffer[3] = 0xF0;

	// Writing to myFooInstance.reg
	myFooInstance.reg.write(buffer, 4);

	// Modifying buffer
	buffer[0] = 0x0F;
	buffer[1] = 0x0F;
	buffer[2] = 0x0F;
	buffer[3] = 0x0F;

	// Writing to myOtherFooInstance.reg
	myOtherFooInstance.reg.write(buffer, 4);

	// Reading from myFooInstance.reg
	myFooInstance.reg.read(buffer, 4);

	// Reading from myOtherFooInstance.reg
	myOtherFooInstance.reg.read(buffer, 4);

//	std::cout << myFooInstance.reg << std::endl;
//	std::cout << myOtherFooInstance.reg << std::endl;

	free(buffer);

}

