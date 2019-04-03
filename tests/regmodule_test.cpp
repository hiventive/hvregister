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
using namespace ::hv::communication::tlm2::protocols::memorymapped;

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
    SomeRegModuleClass(ModuleName name_, ::std::size_t alignment, ::std::size_t regSize)
        : RegModule<>(name_, alignment), reg1(regSize, "reg1", "Register 1", RW),
          reg2(regSize, "reg2", "Register 2", RW), reg3(regSize, "reg3", "Register 3", RW) {
        hvaddr_t addr = 0;
        this->addRegister(addr, reg1);
        addr += HV_BIT_TO_BYTE(regSize);
        addr = mainRegisterFile.getNearestSuperiorAlignedAddress(addr, alignment);
        this->addRegister(addr, reg2);
        addr += HV_BIT_TO_BYTE(regSize);
        addr = mainRegisterFile.getNearestSuperiorAlignedAddress(addr, alignment);
        this->addRegister(addr, reg3);
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

TEST_F(RegModuleTest, AlignmentRWTest1) {
    HV_SYSTEMC_RESET_CONTEXT
    ::sc_core::sc_time zeroTime(::sc_core::SC_ZERO_TIME);

    // alignment: 4 bytes - regSize: 2 bytes
    SomeRegModuleClass rm("RegModuleForAlignmentTest", 4, 16);
    FooMMModule mod("MyFooModule");
    mod.socket.bind(rm.memMapSocket);

    hvuint8_t tmp1[12];
    hvuint8_t tmp2[12];
    for (int i = 0; i < 12; ++i) {
        tmp1[i] = 0xF0;
        tmp2[i] = 0x00;
    }
    // Writing and reading on 32-bit
    MemoryMappedPayload<hvaddr_t> txnWrite;
    txnWrite.setAddress(0x00);
    txnWrite.setDataPtr(tmp1);
    txnWrite.setDataLength(4);
    txnWrite.setCommand(MEM_MAP_WRITE_COMMAND);
    mod.socket->b_transport(txnWrite, zeroTime);
    ASSERT_FALSE(txnWrite.isResponseError());

    MemoryMappedPayload<hvaddr_t> txnRead;
    txnRead.setAddress(0x00);
    txnRead.setDataPtr(tmp2);
    txnRead.setDataLength(4);
    txnRead.setCommand(MEM_MAP_READ_COMMAND);
    mod.socket->b_transport(txnRead, zeroTime);
    ASSERT_FALSE(txnRead.isResponseError());

    ASSERT_EQ(hvuint16_t(rm.getMainRegFile().getRegister(0x00)), hvuint16_t(0xF0F0));
    for (int i = 0; i < 4; ++i) {
        if ((i % 4) < 2) {
            ASSERT_EQ(tmp2[i], hvuint8_t(0xF0));
        } else {
            ASSERT_EQ(tmp2[i], hvuint8_t(0x00));
        }
    }
    // Resetting
    rm.getMainRegFile().getRegister(0x00) = 0;

    // Writing and reading on 3*32-bit
    txnWrite.setAddress(0x00);
    txnWrite.setDataPtr(tmp1);
    txnWrite.setDataLength(12);
    txnWrite.setCommand(MEM_MAP_WRITE_COMMAND);
    mod.socket->b_transport(txnWrite, zeroTime);
    ASSERT_FALSE(txnWrite.isResponseError());

    txnRead.setAddress(0x00);
    txnRead.setDataPtr(tmp2);
    txnRead.setDataLength(12);
    txnRead.setCommand(MEM_MAP_READ_COMMAND);
    mod.socket->b_transport(txnRead, zeroTime);
    ASSERT_FALSE(txnRead.isResponseError());

    ASSERT_EQ(hvuint16_t(rm.getMainRegFile().getRegister(0x00)), hvuint16_t(0xF0F0));
    ASSERT_EQ(hvuint16_t(rm.getMainRegFile().getRegister(0x04)), hvuint16_t(0xF0F0));
    ASSERT_EQ(hvuint16_t(rm.getMainRegFile().getRegister(0x08)), hvuint16_t(0xF0F0));
    for (int i = 0; i < 12; ++i) {
        if ((i % 4) < 2) {
            ASSERT_EQ(tmp2[i], hvuint8_t(0xF0));
        } else {
            ASSERT_EQ(tmp2[i], hvuint8_t(0x00));
        }
    }
    ::sc_core::sc_start();
}

TEST_F(RegModuleTest, AlignmentRWTest2) {
    HV_SYSTEMC_RESET_CONTEXT
    ::sc_core::sc_time zeroTime(::sc_core::SC_ZERO_TIME);

    // alignment: 4 bytes - regSize: 2 bytes
    SomeRegModuleClass rm("RegModuleForAlignmentTest", 4, 48);
    FooMMModule mod("MyFooModule");
    mod.socket.bind(rm.memMapSocket);

    hvuint8_t tmp1[24];
    hvuint8_t tmp2[24];
    for (int i = 0; i < 24; ++i) {
        tmp1[i] = 0xF0;
        tmp2[i] = 0x00;
    }
    // Writing and reading on 32-bit
    MemoryMappedPayload<hvaddr_t> txnWrite;
    txnWrite.setAddress(0x00);
    txnWrite.setDataPtr(tmp1);
    txnWrite.setDataLength(8);
    txnWrite.setCommand(MEM_MAP_WRITE_COMMAND);
    mod.socket->b_transport(txnWrite, zeroTime);
    ASSERT_FALSE(txnWrite.isResponseError());

    MemoryMappedPayload<hvaddr_t> txnRead;
    txnRead.setAddress(0x00);
    txnRead.setDataPtr(tmp2);
    txnRead.setDataLength(8);
    txnRead.setCommand(MEM_MAP_READ_COMMAND);
    mod.socket->b_transport(txnRead, zeroTime);
    ASSERT_FALSE(txnRead.isResponseError());

    ASSERT_EQ(hvuint64_t(rm.getMainRegFile().getRegister(0x00)), hvuint64_t(0xF0F0F0F0F0F0));
    for (int i = 0; i < 8; ++i) {
        if ((i % 8) < 6) {
            ASSERT_EQ(tmp2[i], hvuint8_t(0xF0));
        } else {
            ASSERT_EQ(tmp2[i], hvuint8_t(0x00));
        }
    }
    // Resetting
    rm.getMainRegFile().getRegister(0x00) = 0;

    // Writing and reading on 3*32-bit
    txnWrite.setAddress(0x00);
    txnWrite.setDataPtr(tmp1);
    txnWrite.setDataLength(24);
    txnWrite.setCommand(MEM_MAP_WRITE_COMMAND);
    mod.socket->b_transport(txnWrite, zeroTime);
    ASSERT_FALSE(txnWrite.isResponseError());

    txnRead.setAddress(0x00);
    txnRead.setDataPtr(tmp2);
    txnRead.setDataLength(24);
    txnRead.setCommand(MEM_MAP_READ_COMMAND);
    mod.socket->b_transport(txnRead, zeroTime);
    ASSERT_FALSE(txnRead.isResponseError());

    ASSERT_EQ(hvuint64_t(rm.getMainRegFile().getRegister(0x00)), hvuint64_t(0xF0F0F0F0F0F0));
    ASSERT_EQ(hvuint64_t(rm.getMainRegFile().getRegister(0x08)), hvuint64_t(0xF0F0F0F0F0F0));
    ASSERT_EQ(hvuint64_t(rm.getMainRegFile().getRegister(0x10)), hvuint64_t(0xF0F0F0F0F0F0));
    for (int i = 0; i < 24; ++i) {
        if ((i % 8) < 6) {
            ASSERT_EQ(tmp2[i], hvuint8_t(0xF0));
        } else {
            ASSERT_EQ(tmp2[i], hvuint8_t(0x00));
        }
    }
    ::sc_core::sc_start();
}

