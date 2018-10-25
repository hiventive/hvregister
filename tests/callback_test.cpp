/**
 * @file callback_test.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Apr, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Tests for registerfile.h
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <gtest/gtest.h>
#include <hv/common.h>

using namespace ::hv::common;

template<typename T> class CallbackTest: public ::testing::Test {
};

class Foo {
protected:

	long val;

public:
	Foo() :
			val(0) {
	}

	long getVal() {
		return val;
	}

	long Add(const int &wIn, const long &xIn, const unsigned int &yIn,
			const unsigned long &zIn) {
		val = static_cast<long>(static_cast<long>(wIn) + xIn
				+ static_cast<long>(yIn) + static_cast<long>(zIn));
		return val;
	}

};

long mul(const int &wIn, const long &xIn, const unsigned int &yIn,
		const unsigned long &zIn) {
	return static_cast<long>(static_cast<long>(wIn) * xIn
			* static_cast<long>(yIn) * static_cast<long>(zIn));
}

long addMul(const int &wIn, const long &xIn, const unsigned int &yIn,
		const unsigned long &zIn) {
	return static_cast<long>(static_cast<long>(wIn)
			+ xIn * static_cast<long>(yIn) + static_cast<long>(zIn));
}

TEST(CallbackTest, CallbackCreationTest) {
	// Function types declarations (std::function and classical function)
	typedef std::function<
			long(const int&, const long&, const unsigned int&,
					const unsigned long&)> stdfunc_t;
	typedef CallbackImpl<
			long(const int&, const long&, const unsigned int&,
					const unsigned long&)> cb_t;

	Foo fooVal;

	cb_t addCb(&Foo::Add, &fooVal);
	cb_t mulCb(mul);

	stdfunc_t addMulStdFunc(addMul);
	cb_t addMulCb(addMulStdFunc);

	int w = 1;
	unsigned int x = 2;
	long y = 3;
	unsigned long z = 4;
	long tmp = addCb(w, x, y, z);

	ASSERT_EQ(tmp,1 + 2 + 3 + 4)<< "Error applying addCb";
	ASSERT_EQ(fooVal.getVal(), 1 + 2 + 3 + 4)<< "Error in modification of val member of Foo";

	ASSERT_EQ(mulCb(w, x, y, z), 1*2*3*4)<< "Error applying mulCb";
	ASSERT_EQ(addMulCb(w, x, y, z), 1+2*3+4)<< "Error applying addMulCB";

}

TEST(CallbackTest, CallbackVectorTest) {
	// Function types declarations (std::function and classical function)
	typedef std::function<
			long(const int&, const long&, const unsigned int&,
					const unsigned long&)> stdfunc_t;
	typedef CallbackImpl<
			long(const int&, const long&, const unsigned int&,
					const unsigned long&)> cb_t;
	typedef std::vector<
			CallbackImpl<long(const int&, const long&, const unsigned int&,
					const unsigned long&)>> cbv_t;

	Foo fooVal;

	cb_t addCb(&Foo::Add, &fooVal);
	cb_t mulCb(mul);

	stdfunc_t addMulStdFunc(addMul);
	cb_t addMulCb(addMulStdFunc);

	int w = 1;
	unsigned int x = 2;
	long y = 3;
	unsigned long z = 4;

	cbv_t myCbv;

	ASSERT_TRUE(myCbv.empty())<< "Callback vector should be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(0))<< "Callback vector should be of size 0";
	myCbv.push_back(mulCb);
	ASSERT_FALSE(myCbv.empty())<< "Callback vector should not be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(1))<< "Callback vector should be of size 1";

	auto itTmp = myCbv.begin();
	myCbv.insert(itTmp, addCb);

	ASSERT_FALSE(myCbv.empty())<< "Callback vector should not be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(2))<< "Callback vector should be of size 2";

	myCbv.push_back(addMulCb);

	ASSERT_FALSE(myCbv.empty())<< "Callback vector should not be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(3))<< "Callback vector should be of size 3";

	// Calling functions by iterator
	long res[3];
	int ind = 0;
	for (auto it = myCbv.begin(); it != myCbv.end(); ++it) {
		res[ind] = (*it)(w, x, y, z);
		ind++;
	}

	ASSERT_EQ(res[0], 1+2+3+4)<< "Error in addition function";
	ASSERT_EQ(res[1], 1*2*3*4)<< "Error in multiplication function";
	ASSERT_EQ(res[2], 1+2*3+4)<< "Error in addition/multiplication function";

	// Calling functions by 'at' method
	ASSERT_EQ(myCbv.at(0)(w, x, y, z), 1+2+3+4)<< "Error in addition function";
	ASSERT_EQ(myCbv.at(1)(w, x, y, z), 1*2*3*4)<< "Error in multiplication function";
	ASSERT_EQ(myCbv.at(2)(w, x, y, z), 1+2*3+4)<< "Error in addition/multiplication function";

	// Calling functions by [] operator
	ASSERT_EQ(myCbv[0](w, x, y, z), 1+2+3+4)<< "Error in addition function";
	ASSERT_EQ(myCbv[1](w, x, y, z), 1*2*3*4)<< "Error in multiplication function";
	ASSERT_EQ(myCbv[2](w, x, y, z), 1+2*3+4)<< "Error in addition/multiplication function";

	//Erasing first element
	itTmp = myCbv.begin();
	myCbv.erase(itTmp);
	ASSERT_FALSE(myCbv.empty())<< "Callback vector should not be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(2))<< "Callback vector should be of size 2";

	// Erasing second element
	itTmp = myCbv.begin();
	myCbv.erase(itTmp);
	ASSERT_FALSE(myCbv.empty())<< "Callback vector should not be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(1))<< "Callback vector should be of size 1";

	// Erasing last element
	itTmp = myCbv.begin();
	myCbv.erase(itTmp);
	ASSERT_TRUE(myCbv.empty())<< "Callback vector should be empty";
	ASSERT_EQ(myCbv.size(),std::size_t(0))<< "Callback vector should be of size 0";

}

class MasterClass {
protected:
	// Type definitions for callbacks
	typedef CallbackImpl<int(const int&, std::string&)> cb_t;
	typedef std::vector<CallbackImpl<int(const int&, std::string&)>> cbv_t;

	// Main data member
	int value;

	// Monitoring helpers
	unsigned int cpt;
	std::string str;

	// Callback vector
	cbv_t myCallbackVector;

public:
	// Constructor
	MasterClass(const int &valueIn) :
			value(valueIn), cpt(1), str(
					"0 - Initialization - value = " + std::to_string(valueIn)
							+ "\n") {
	}

	// Destructor
	virtual ~MasterClass() {
	}

	// Accessor to value
	int getValue() {
		return value;
	}

	// Method to register callback to myCallbackVector
	void registerCallback(const cb_t &callback) {
		this->myCallbackVector.push_back(callback);
	}

	// Accessor to myCallbackVector's size
	std::size_t howManyCallbacks() const {
		return this->myCallbackVector.size();
	}

	// Method using callbacks
	std::string superMethod() {
		this->str += std::to_string(cpt++) + " - Entering superMethod()\n"; // Tracking

		// Checking if there are registered callbacks
		if (!myCallbackVector.empty()) {
			// Iterating on callbacks
			for (auto it = myCallbackVector.begin();
					it != myCallbackVector.end(); ++it) {

				std::string strTmp(std::to_string(cpt++) + " - "); // Tracking

				// Invoking callback
				this->value = (*it)(this->value, strTmp);

				this->str += strTmp + " - value = " + std::to_string(this->value)
						+ "\n"; // Tracking
			}
		}
		this->str += std::to_string(cpt++) + " - Leaving superMethod()\n"; // Tracking
		return this->str;
	}
};

int addFive(const int &intIn, std::string &strIn) {
	strIn += "Adding 5"; // Tracking
	return intIn + 5;
}

class MultiplyClass {
protected:
	int multiplier;
	int result;

public:
	MultiplyClass(const int &multiplierIn) :
			multiplier(multiplierIn), result(0) {
	}

	virtual ~MultiplyClass() {
	}

	int getResult() {
		return result;
	}

	int multiplyByMultiplier(const int &intIn, std::string &strIn) {
		strIn += "Multiplying by " + std::to_string(multiplier); // Tracking
		this->result = intIn * this->multiplier;
		return this->result;
	}
};

TEST(CallbackTest, StartingUpGuideTest) {
	// Creating an instance of MasterClass
	MasterClass masterInstance(11);

	// Registering addFive function as a callback
	masterInstance.registerCallback(addFive);

	// Creating an instance of MultiplyClass
	MultiplyClass multiplyInstance(-2);
	// Registering multiplyByMultiplier method as a callback
	masterInstance.registerCallback(
			std::bind(&MultiplyClass::multiplyByMultiplier, &multiplyInstance,
					std::placeholders::_1, std::placeholders::_2));

	// Registering a lambda function adding 32
	masterInstance.registerCallback([](const int &intIn, std::string &strIn) {
		strIn += "Adding 32"; // Tracking
			return intIn + 32;
		});

	// Calling superMethod
	std::string ret = masterInstance.superMethod();

	// Displaying output
	std::cout << ret << std::endl;

	ASSERT_EQ(masterInstance.getValue(), 0);
	ASSERT_EQ(multiplyInstance.getResult(), -32);

	// Calling superMethod
	ret = masterInstance.superMethod();
	// Displaying output
	std::cout << ret << std::endl;

	ASSERT_EQ(masterInstance.getValue(), 22);

}

//
//#define WIDTH 32
//#define SIZE WIDTH/8
//
//using namespace ::hv::common;
//using namespace ::hv::reg;
//
//enum funcCalled {
//	PRE_READ_CLASS_METHOD,
//	POST_READ_CLASS_METHOD,
//	PRE_WRITE_CLASS_METHOD,
//	POST_WRITE_CLASS_METHOD,
//	PRE_READ_FUNCTION,
//	POST_READ_FUNCTION,
//	PRE_WRITE_FUNCTION,
//	POST_WRITE_FUNCTION,
//	PRE_READ_LAMBDA,
//	POST_READ_LAMBDA,
//	PRE_WRITE_LAMBDA,
//	POST_WRITE_LAMBDA,
//};
//
//std::vector<funcCalled> whoWasCalled;
//
//template<typename T> class CallbackTest: public ::testing::Test {
//};
//
//template<std::size_t BIT_WIDTH, typename DATA_TYPE> class readFoo {
//public:
//	readFoo() :
//			val(0) {
//	}
//
//	DATA_TYPE getVal() {
//		return val;
//	}
//
//	bool myPreFunc(const RegisterReadEvent<BIT_WIDTH, DATA_TYPE> &ev) {
//		whoWasCalled.push_back(funcCalled::PRE_READ_CLASS_METHOD);
//		this->val = ev.value;
//		return true;
//	}
//
//	void myPostFunc(const RegisterReadEvent<BIT_WIDTH, DATA_TYPE> &ev) {
//		whoWasCalled.push_back(funcCalled::POST_READ_CLASS_METHOD);
//		this->val = ev.value;
//	}
//
//protected:
//	DATA_TYPE val;
//};
//
//template<std::size_t BIT_WIDTH, typename DATA_TYPE> class writeFoo {
//public:
//	writeFoo() :
//			oldVal(0), newVal(0) {
//	}
//
//	DATA_TYPE getOldVal() {
//		return oldVal;
//	}
//
//	DATA_TYPE getNewVal() {
//		return newVal;
//	}
//
//	bool myPreFunc(const RegisterWriteEvent<BIT_WIDTH, DATA_TYPE> &ev) {
//		whoWasCalled.push_back(funcCalled::PRE_WRITE_CLASS_METHOD);
//		this->oldVal = ev.oldValue;
//		this->newVal = ev.newValue;
//		return true;
//	}
//
//	void myPostFunc(const RegisterWriteEvent<BIT_WIDTH, DATA_TYPE> &ev) {
//		whoWasCalled.push_back(funcCalled::POST_WRITE_CLASS_METHOD);
//		this->oldVal = ev.oldValue;
//		this->newVal = ev.newValue;
//	}
//
//protected:
//	DATA_TYPE oldVal;
//	DATA_TYPE newVal;
//};
//
//bool barPreReadFunc(const RegisterReadEvent<32, BitVector<32>> &ev) {
//	whoWasCalled.push_back(funcCalled::PRE_READ_FUNCTION);
//	return true;
//}
//
//void barPostReadFunc(const RegisterReadEvent<32, BitVector<32>> &ev) {
//	whoWasCalled.push_back(funcCalled::POST_READ_FUNCTION);
//}
//
//bool barPreWriteFunc(const RegisterWriteEvent<32, BitVector<32>> &ev) {
//	whoWasCalled.push_back(funcCalled::PRE_WRITE_FUNCTION);
//	return true;
//}
//
//void barPostWriteFunc(const RegisterWriteEvent<32, BitVector<32>> &ev) {
//	whoWasCalled.push_back(funcCalled::POST_WRITE_FUNCTION);
//}
//
//TEST(CallbackTest, CallbackRegistrationTest) {
//	// Register creation
//	Register<WIDTH> reg("Register name", "Register description", READ_WRITE);
//
//	// Registering read callbacks
//	typedef readFoo<WIDTH, BitVector<WIDTH>> readfoo_t;
//	readfoo_t rf;
//	ASSERT_EQ(reg.getNPreReadCallbacks(), std::size_t(0));
//	reg.registerPreReadCallback(
//			std::bind(&readfoo_t::myPreFunc, &rf, std::placeholders::_1));
//	ASSERT_EQ(reg.getNPreReadCallbacks(), std::size_t(1));
//	reg.registerPreReadCallback(barPreReadFunc);
//	ASSERT_EQ(reg.getNPreReadCallbacks(), std::size_t(2));
//	reg.registerPreReadCallback(
//			[](const RegisterReadEvent<WIDTH, BitVector<WIDTH>>&) {whoWasCalled.push_back(funcCalled::PRE_READ_LAMBDA); return true;});
//	ASSERT_EQ(reg.getNPreReadCallbacks(), std::size_t(3));
//
//	ASSERT_EQ(reg.getNPostReadCallbacks(), std::size_t(0));
//	reg.registerPostReadCallback(
//			std::bind(&readfoo_t::myPostFunc, &rf, std::placeholders::_1));
//	ASSERT_EQ(reg.getNPostReadCallbacks(), std::size_t(1));
//	reg.registerPostReadCallback(barPostReadFunc);
//	ASSERT_EQ(reg.getNPostReadCallbacks(), std::size_t(2));
//	reg.registerPostReadCallback(
//			[](const RegisterReadEvent<WIDTH, BitVector<WIDTH>>&) {whoWasCalled.push_back(funcCalled::POST_READ_LAMBDA);});
//	ASSERT_EQ(reg.getNPostReadCallbacks(), std::size_t(3));
//
//	// Registering write callbacks
//	typedef writeFoo<WIDTH, BitVector<WIDTH>> writefoo_t;
//	writefoo_t wf;
//	ASSERT_EQ(reg.getNPreWriteCallbacks(), std::size_t(0));
//	reg.registerPreWriteCallback(
//			std::bind(&writefoo_t::myPreFunc, &wf, std::placeholders::_1));
//	ASSERT_EQ(reg.getNPreWriteCallbacks(), std::size_t(1));
//	reg.registerPreWriteCallback(barPreWriteFunc);
//	ASSERT_EQ(reg.getNPreWriteCallbacks(), std::size_t(2));
//	reg.registerPreWriteCallback(
//			[](const RegisterWriteEvent<WIDTH, BitVector<WIDTH>>&) {whoWasCalled.push_back(funcCalled::PRE_WRITE_LAMBDA); return true;});
//	ASSERT_EQ(reg.getNPreWriteCallbacks(), std::size_t(3));
//
//	ASSERT_EQ(reg.getNPostWriteCallbacks(), std::size_t(0));
//	reg.registerPostWriteCallback(
//			std::bind(&writefoo_t::myPostFunc, &wf, std::placeholders::_1));
//	ASSERT_EQ(reg.getNPostWriteCallbacks(), std::size_t(1));
//	reg.registerPostWriteCallback(barPostWriteFunc);
//	ASSERT_EQ(reg.getNPostWriteCallbacks(), std::size_t(2));
//	reg.registerPostWriteCallback(
//			[](const RegisterWriteEvent<WIDTH, BitVector<WIDTH>>&) {whoWasCalled.push_back(funcCalled::POST_WRITE_LAMBDA);});
//	ASSERT_EQ(reg.getNPostWriteCallbacks(), std::size_t(3));
//
//	// Writing value
//	hvuint8_t writeBuff[SIZE];
//	for (std::size_t i = 0; i < SIZE; i++) {
//		writeBuff[i] = 0x55; //01010101
//	}
//	bool retWrite = reg.write(writeBuff, SIZE);
//	ASSERT_TRUE(retWrite)<< "Error while writing in register";
//
//	// Reading value
//	hvuint8_t readBuff[SIZE];
//	bool retRead = reg.read(readBuff, SIZE);
//	ASSERT_TRUE(retRead)<< "Error while reading from register";
//	for (std::size_t i = 0; i < SIZE; i++) {
//		ASSERT_EQ(readBuff[i], hvuint8_t(0x55))<< "Error in read data (buff position " << i << ")";
//	}
//
//	// Checking callbacks stats
//	ASSERT_EQ(whoWasCalled.size(), std::size_t(12))<< "Error in the number of callbacks called";
//
//	ASSERT_EQ(whoWasCalled[0],funcCalled::PRE_WRITE_CLASS_METHOD)<< "Error in 1st pre-write";
//	ASSERT_EQ(whoWasCalled[1],funcCalled::PRE_WRITE_FUNCTION)<< "Error in 2nd pre-write";
//	ASSERT_EQ(whoWasCalled[2],funcCalled::PRE_WRITE_LAMBDA)<< "Error in 3rd pre-write";
//
//	ASSERT_EQ(whoWasCalled[3],funcCalled::POST_WRITE_CLASS_METHOD)<< "Error in 1st post-write";
//	ASSERT_EQ(whoWasCalled[4],funcCalled::POST_WRITE_FUNCTION)<< "Error in 2nd post-write";
//	ASSERT_EQ(whoWasCalled[5],funcCalled::POST_WRITE_LAMBDA)<< "Error in 3rd post-write";
//
//	ASSERT_EQ(whoWasCalled[6],funcCalled::PRE_READ_CLASS_METHOD)<< "Error in 1st pre-read";
//	ASSERT_EQ(whoWasCalled[7],funcCalled::PRE_READ_FUNCTION)<< "Error in 2nd pre-read";
//	ASSERT_EQ(whoWasCalled[8],funcCalled::PRE_READ_LAMBDA)<< "Error in 3rd pre-Read";
//
//	ASSERT_EQ(whoWasCalled[9],funcCalled::POST_READ_CLASS_METHOD)<< "Error in 1st post-read";
//	ASSERT_EQ(whoWasCalled[10],funcCalled::POST_READ_FUNCTION)<< "Error in 2nd post-read";
//	ASSERT_EQ(whoWasCalled[11],funcCalled::POST_READ_LAMBDA)<< "Error in 3rd post-read";
//
//}
//
