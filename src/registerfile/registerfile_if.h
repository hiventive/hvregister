/**
 * @file registerfile_if.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief RegisterFile Interface class
 */
#ifndef HV_REGISTERFILE_IF_H_
#define HV_REGISTERFILE_IF_H_

#include <hv/common.h>

#include "../cci/register_if.h"

namespace hv {
namespace reg {

template<typename REG_TYPE> class RegisterFileIf {
public:
//** Destructor **//
	virtual ~RegisterFileIf() {
	}

//** Accessors **//
	/**
	 * Get register file name
	 * @return Register file name
	 */
	virtual std::string getName() const = 0;

	/**
	 * Get register file description
	 * @return Register file description
	 */
	virtual std::string getDescription() const = 0;

	/**
	 * Get alignment in bytes
	 * @return Alignment in bytes
	 */
	virtual std::size_t getAlignment() const = 0;

//** Register insertion and creation **//
	/**
	 * Add register to given address by reference
	 * @param insertAddr Address
	 * @param reg Reference to register to be added to register file
	 * @return True if success, false else
	 */
	virtual bool addRegister(const ::hv::common::hvaddr_t &insertAddr, REG_TYPE &reg) = 0;

	/**
	 * Create and add register
	 * @param address Address where to add register
	 * @param sizeIn Register size in bits
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 * @param RWModeIn Register read/write mode
	 * @param resetIn Register reset value
	 * @return true if register was successfully inserted, false else.
	 */
	virtual bool createRegister(const ::hv::common::hvaddr_t &address,
			const std::size_t &sizeIn, const std::string &nameIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &RWModeIn) = 0;

	/**
	 * Create and add a register block
	 * The first register is inserted at specified address if conditions are OK.
	 * Following registers are inserted a compact way to successive aligned addresses
	 * All register names are modified adding "_0" for the first one, "_1" for the second one, etc.
	 * All other parameters are the same for all registers.
	 * @param address Address where to insert first register
	 * @param nBlock Number of registers to create
	 * @param sizeIn Register size in bits
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 * @param RWModeIn Register read/write mode
	 * @return true if registers were successfully inserted, false else.
	 */
	virtual bool createRegisterBlock(const ::hv::common::hvaddr_t &address, const std::size_t &nBlock,
			const std::size_t &sizeIn, const std::string &nameIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &RWModeIn) = 0;

//** Register access **//
	/**
	 * Operator () overloading - Get reference to register by address
	 * @param address Register's address
	 * @return Reference to register
	 */
	virtual REG_TYPE& operator()(const ::hv::common::hvaddr_t &address) const = 0;

	/**
	 * Operator () overloading - Get reference to register by name
	 * @param name Register's name
	 * @return Reference to register
	 */
	virtual REG_TYPE& operator()(const std::string &name) const = 0;

//** Read/Write methods **//
	/**
	 * Read from register in register file
	 * @param address Address of register
	 * @param readBuff Read buffer
	 * @param readSize Read size
	 */
	virtual bool read(const ::hv::common::hvaddr_t &address, ::hv::common::hvuint8_t* readBuff,
			const std::size_t &readSize) = 0;

	/**
	 * Write to register in register file
	 * @param address Address of register
	 * @param writeBuff Write buffer
	 * @param writeSize Write size
	 */
	virtual bool write(const ::hv::common::hvaddr_t &address, const ::hv::common::hvuint8_t* writeBuff,
			const std::size_t &writeSize) = 0;

};

} // namespace reg
} // namespace hv

#endif /* HV_REGISTERFILE_IF_H_ */
