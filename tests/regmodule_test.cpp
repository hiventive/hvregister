/**
 * @file reg_module.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date May, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register-style module
 */
#include <iostream>
#include <regmodule/reg_module.h>
#include <gtest/gtest.h>
#include <systemc>

using namespace ::hv::common;
using namespace ::hv::module;
using namespace ::hv::reg;

class RegModuleTest : public ::testing::Test {
  protected:
    class MyRegModule : public RegModule<32> {
      public:
        MyRegModule(ModuleName name_)
            : RegModule(name_, 4), // Alignment
              reg1(8, "Reg1", "Register 1", RO), reg2(16, "Reg2", "Register 2", WO),
              reg3(32, "Reg3", "Register 3", RW) {
            addRegister(reg1);
            addRegister(0x8, reg2);
            addRegister(0x10, reg3);
        }

        Register &getReg1() {
            return reg1;
        }

        Register &getReg2() {
            return mainRegisterFile.getRegister(0x8);
        }

        Register &getReg3() {
            return mainRegisterFile.getRegister("Reg3");
        }

        void setReg1Val(hvuint8_t val) {
            reg1 = val;
        }

        hvuint8_t getReg1Val() {
            return reg1.getValue();
        }

      protected:
        Register reg1;
        Register reg2;
        Register reg3;
    };

    virtual void SetUp() {
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test
        // (right before the destructor).
    }
};

class FooMMModule : public ::hv::module::Module {
  public:
    FooMMModule(::hv::module::ModuleName name_) : ::hv::module::Module(name_) {
    }
    ::hv::communication::tlm2::protocols::memorymapped::MemoryMappedSimpleInitiatorSocket<> socket;
};

// Class to test inserting 16-bit registers in a 32-bit-aligned memory space
class SomeRegModuleClass : public RegModule<> {
  public:
    SomeRegModuleClass(ModuleName name_)
        : RegModule<>(name_, 4), reg1(16, "reg1", "Register 1", RW),
          reg2(16, "reg2", "Register 2", RW), reg3(16, "reg3", "Register 3", RW) {
              this->addRegister(0x00, reg1);
              this->addRegister(0x04, reg2);
              this->addRegister(0x08, reg3);
    }

    virtual ~SomeRegModuleClass() {
    }

  protected:
    Register reg1;
    Register reg2;
    Register reg3;
};

TEST_F(RegModuleTest, InstantiationTest) {
    HV_SYSTEMC_RESET_CONTEXT
    MyRegModule rm("myRM");
    FooMMModule mod("myFooModule");

    Register reg1 = rm.getReg1();
    Register reg2 = rm.getReg2();
    Register reg3 = rm.getReg3();

    mod.socket(rm.memMapSocket);

    ASSERT_STREQ(reg1.getName().c_str(), std::string("Reg1").c_str());
    ASSERT_STREQ(reg2.getName().c_str(), std::string("Reg2").c_str());
    ASSERT_STREQ(reg3.getName().c_str(), std::string("Reg3").c_str());

    ::sc_core::sc_start();
}

TEST_F(RegModuleTest, ModuleInsertionTest) {
    HV_SYSTEMC_RESET_CONTEXT
    MyRegModule rm1("myRM1"), rm2("myRM2");
    FooMMModule mod1("myFooModule1"), mod2("myFooModule2");
    rm1.addRegModule(0x14, rm2);

    rm2.setReg1Val(0xF0);

    RegisterFile &rfTmp = rm1.getMainRegFile();
    hvuint8_t val = rfTmp.getRegister(0x14).getValue();

    ASSERT_EQ(val, static_cast<hvuint8_t>(0xF0));

    mod1.socket(rm1.memMapSocket);
    mod2.socket(rm2.memMapSocket);

    ::sc_core::sc_start();
}

TEST_F(RegModuleTest, AlignmentRWTest) {
    HV_SYSTEMC_RESET_CONTEXT

    SomeRegModuleClass rm("RegModuleForAlignmentTest");
    FooMMModule mod("MyFooModule");
    mod.socket.bind(rm.memMapSocket); // Just for TLM to be happy
    
    hvuint8_t tmp1[12] = {0xF0};
    hvuint8_t tmp2[12] = {0};
    rm.getMainRegFile().write(0, tmp1, 4);
    rm.getMainRegFile().read(0, tmp2, 4);
    Register& reg1Tmp = rm.getMainRegFile().getRegister(0);
    hvuint16_t reg1Val = reg1Tmp;
    ASSERT_EQ(reg1Val, hvuint16_t(0xF0F0));
    ASSERT_EQ(tmp2[0], hvuint16_t(0xF0));
    ASSERT_EQ(tmp2[1], hvuint16_t(0xF0));
    ASSERT_EQ(tmp2[2], hvuint16_t(0x00));
    ASSERT_EQ(tmp2[3], hvuint16_t(0x00));

    ::sc_core::sc_start();
}
