/**
 * @file registerfile.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief RegisterFile class
 */

#ifndef HV_REGISTERFILE_H_
#define HV_REGISTERFILE_H_

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <hv/common.h>

#include "registerfile_if.h"
#include "../register/register.h"

namespace hv {
namespace reg {

class RegisterFile: public RegisterFileIf<Register> {
public:
//** Type definitions **//
	typedef std::map<::hv::common::hvaddr_t, Register&> rmap_t;
	typedef std::map<::hv::common::hvaddr_t, RegisterFile&> rfmap_t;

//** Constructors **//
	/**
	 * Register file constructor
	 * @param nameIn Register file name
	 * @param descriptionIn Register file description
	 * @param alignmentIn Register file alignment in bytes
	 */
	RegisterFile(std::string nameIn, std::string descriptionIn,
			std::size_t alignmentIn = 0);

	/**
	 * RegisterFile copy constructor
	 * All children registers and register files are cloned (not only their references)
	 * @param src Source of copy
	 */
	RegisterFile(const RegisterFile &src);

//** Destructors **//
	virtual ~RegisterFile();

//** Accessors **//
	/**
	 * Get register file name
	 * @return Register file name
	 */
	std::string getName() const;

	/**
	 * Get register file description
	 * @return Register file description
	 */
	std::string getDescription() const;

	/**
	 * Get alignment in bytes
	 * @return Alignment in bytes
	 */
	std::size_t getAlignment() const;

	/**
	 * Returns 0 if size is not fixed yet (current RegisterFile
	 * was not insterted in a parent RegisterFile)
	 * Else, returns the size which was fixed during insertion
	 * @return
	 */
	std::size_t getFixedSize() const;

	/**
	 * Check if current register file is empty
	 * @return true if empty, false else
	 */
	bool isEmpty() const;

//** Register file's memory management **//

	/**
	 * Get the end address of an element starting at address startAddr and of size blockSize bytes
	 * @param startAddr Starting address
	 * @param blockSize Size in bytes
	 * @return End address
	 */
	::hv::common::hvaddr_t getEndAddress(const ::hv::common::hvaddr_t &startAddr,
			const std::size_t &blockSize) const;

	/**
	 * Checks if two blocks defined by their starting address and end address do recover
	 * @param startAddr1 Starting address of block 1
	 * @param blockSize1 Size of block 1 in bytes
	 * @param startAddr2 Starting address of block 2
	 * @param blockSize2 Size of block 2 in bytes
	 * @return true if blocks do recover
	 */
	bool recover(const ::hv::common::hvaddr_t &startAddr1, const std::size_t &blockSize1,
			const ::hv::common::hvaddr_t &startAddr2, const std::size_t &blockSize2) const;

	/**
	 * Get last occupied address in current registerFile
	 * @return Last occupied address in current registerFile
	 */
	::hv::common::hvaddr_t getLastOccupiedAddress() const;

	/**
	 * Checks if the block starting at startAddr and of size blockSize bytes is free for insertion
	 * @param startAddr Starting block address
	 * @param blockSize Block size in bytes
	 * @return true if space is free for insertion, false else
	 */
	bool isSpaceFree(const ::hv::common::hvaddr_t &startAddr,
			const std::size_t &blockSize) const;

	/**
	 * Returns true if startAddress is a valid aligned address for an alignment alignSize
	 * @param startAddr Element starting address
	 * @param alignSize Alignment size in bytes
	 * @return true if address is valid for given alignment
	 */
	bool isAligned(const ::hv::common::hvaddr_t &startAddr,
			const std::size_t &alignSize) const;
	/**
	 * Returns nearest superior aligned address from addr considering alignment align
	 * @param addr Reference address
	 * @param alignSize Reference alignment
	 * @return Nearest superior aligned address
	 */
	::hv::common::hvaddr_t getNearestSuperiorAlignedAddress(const ::hv::common::hvaddr_t &addr,
			const std::size_t &alignSize) const;

//** Register insertion and creation **//
	/**
	 * Add register to given address by reference
	 * @param insertAddr Address
	 * @param reg Reference to register to be added to register file
	 * @return True if success, false else
	 */
	bool addRegister(const ::hv::common::hvaddr_t &insertAddr, Register &reg);

	/**
	 * Add register by reference with no address specified
	 * @param reg Reference to register to be added to register file
	 * @return True if success, false else
	 */
	bool addRegister(Register &reg);

	/**
	 * Adds register by copy to specified address
	 * @param insertAddr Address where to insert register copy
	 * @param reg Register to copy
	 * @return True if success, false else
	 */
	bool addRegisterCopy(const ::hv::common::hvaddr_t &insertAddr, const Register &reg);

	/**
	 * Adds register by copy to unspecified address
	 * @param reg Register to copy
	 * @return True if success, false else
	 */
	bool addRegisterCopy(const Register &reg);

	/**
	 * Create and add register
	 * @param address Address where to add register
	 * @param sizeIn Register size in bits
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 * @param RWModeIn Register read/write mode
	 * @return true if register was successfully inserted, false else.
	 */
	bool createRegister(const ::hv::common::hvaddr_t &address, const std::size_t &sizeIn,
			const std::string &nameIn, const std::string &descriptionIn,
			const ::hv::common::hvrwmode_t &RWModeIn);

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
	bool createRegister(const ::hv::common::hvaddr_t &address, const std::size_t &sizeIn,
			const std::string &nameIn, const std::string &descriptionIn,
			const ::hv::common::hvrwmode_t &RWModeIn, const ::hv::common::BitVector &resetIn);

	/**
	 * Create and add register
	 * No address specified: register will be created at the end of space, considering alignment value.
	 * @param sizeIn Register size in bits
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 * @param RWModeIn Register read/write mode
	 * @param resetIn Register reset value
	 * @return true if register was successfully inserted, false else.
	 */
	bool createRegister(const std::size_t &sizeIn, const std::string &nameIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &RWModeIn,
			const ::hv::common::BitVector &resetIn = 0u);

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
	bool createRegisterBlock(const ::hv::common::hvaddr_t &address, const std::size_t &nBlock,
			const std::size_t &sizeIn, const std::string &nameIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &RWModeIn);

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
	 * @param resetIn Register reset value
	 * @return true if registers were successfully inserted, false else.
	 */
	bool createRegisterBlock(const ::hv::common::hvaddr_t &address, const std::size_t &nBlock,
			const std::size_t &sizeIn, const std::string &nameIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &RWModeIn,
			const ::hv::common::BitVector &resetIn);

	/**
	 * Create and add a register block
	 * No address specified: register will be created at the end of space, considering alignment value.
	 * The first register is inserted at specified address if conditions are OK.
	 * Following registers are inserted a compact way to successive aligned addresses
	 * All register names are modified adding "_0" for the first one, "_1" for the second one, etc.
	 * All other parameters are the same for all registers.
	 * @param nBlock Number of registers to create
	 * @param sizeIn Register size in bits
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 * @param RWModeIn Register read/write mode
	 * @param resetIn Register reset value
	 * @return true if registers were successfully inserted, false else.
	 */
	bool createRegisterBlock(const std::size_t &nBlock,
			const std::size_t &sizeIn, const std::string &nameIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &RWModeIn,
			const ::hv::common::BitVector &resetIn = 0u);

//** Add register file to register file **//
	/**
	 * Adds register file by reference to specified address
	 * @param insertAddr Insertion address
	 * @param regFile Reference to register file to insert
	 * @return True if register file was correctly inserted, false else.
	 */
	bool addRegisterFile(const ::hv::common::hvaddr_t &insertAddr, RegisterFile &regFile);

	/**
	 * Adds register file by reference to specified address
	 * @param insertAddr Insertion address
	 * @param regFile Reference to register file to insert
	 * @param regFileSize Fixed register file size in bytes.
	 * By default, its width is defined by its highest occupied address.
	 * @return True if register file was correctly inserted, false else.
	 */
	bool addRegisterFile(const ::hv::common::hvaddr_t &insertAddr, RegisterFile &regFile,
			const std::size_t &regFileSize);

	/**
	 * Adds register file by reference with no specified address
	 * @param regFile Reference to register file to insert
	 * @return True if register file was correctly inserted, false else.
	 */
	bool addRegisterFile(RegisterFile &regFile);

	/**
	 * Adds register file by reference with no specified address
	 * Alignment is the alignment of current RegisterFile if defined
	 * or from the size of the RegisterFile to insert
	 * @param regFile Reference to register file to insert
	 * @param regFileSize Fixed register file size in bytes.
	 * By default, its width is defined by its highest occupied address.
	 * @return True if register file was correctly inserted, false else.
	 */
	bool addRegisterFile(RegisterFile &regFile, const std::size_t &regFileSize);

//** Children accessors and helpers **//
	/**
	 * Get register address from its name
	 *
	 * If several registers have the same name, the address of the one
	 * at the smallest address is returned.
	 * In general, it is advised to manipulate registers by address once inserted
	 * in a RegisterFile.
	 * @param name Register's name
	 * @return Address corresponding to register name
	 */
	::hv::common::hvaddr_t getRegisterAddress(const std::string &name) const;

	/**
	 * Get register file address from its name
	 *
	 * If several register files have the same name, the address of the one
	 * at the smallest address is returned.
	 * In general, it is advised to manipulate inner RegisterFiles by address once inserted
	 * in a RegisterFile.
	 * @param name Register's name
	 * @return Address corresponding to register name
	 */
	::hv::common::hvaddr_t getRegisterFileAddress(const std::string &name) const;

	/**
	 * Get reference to register in current register file from its address (recursive)
	 * @param address Address of desired register
	 * @return Reference to desired register
	 */
	Register& getRegister(const ::hv::common::hvaddr_t &address) const;

	/**
	 * Get reference to register in current register file from its name
	 *
	 * If several registers have the same name, a reference to the one
	 * at the smallest address is returned.
	 * In general, it is advised to manipulate registers by address once inserted
	 * in a RegisterFile.
	 * @param name Name of desired register
	 * @return Reference to desired register
	 */
	Register& getRegister(const std::string &name) const;

	/**
	 * Get reference to register file in current register file from its address
	 * @param address Address of desired register file
	 * @return Reference to desired register file
	 */
	RegisterFile& getRegisterFile(const ::hv::common::hvaddr_t &address) const;

	/**
	 * Get reference to register file in current register file from its name
	 * @param name Name of desired register file
	 * @return Reference to desired register file
	 */
	RegisterFile& getRegisterFile(const std::string &name) const;

//** Access operators overloading **//
	/**
	 * Operator () overloading -> equivalent to getRegister
	 * @param address Register's address
	 * @return Reference to register
	 */
	Register& operator()(const ::hv::common::hvaddr_t &address) const;

	/**
	 * Operator () overloading -> equivalent to getRegister
	 * @param name Register's name
	 * @return Reference to register
	 */
	Register& operator()(const std::string &name) const;

	/**
	 * Operator [] overloading -> equivalent to getRegisterFile
	 * @param address Child RegisterFile's address
	 * @return Reference to RegisterFile
	 */
	RegisterFile& operator [](const ::hv::common::hvaddr_t &address) const;

	/**
	 * Operator [] overloading -> equivalent to getRegisterFile
	 * @param name Child RegisterFile's name
	 * @return Reference to RegisterFile
	 */
	RegisterFile& operator [](const std::string &name) const;

//** Read/Write methods **//
	/**
	 * Read from register in register file
	 * @param address Address of register
	 * @param readBuff Read buffer
	 * @param readSize Read size
	 */
	bool read(const ::hv::common::hvaddr_t &address, ::hv::common::hvuint8_t* readBuff,
			const std::size_t &readSize);

	/**
	 * Write to register in register file
	 * @param address Address of register
	 * @param writeBuff Write buffer
	 * @param writeSize Write size
	 */
	bool write(const ::hv::common::hvaddr_t &address, const ::hv::common::hvuint8_t* writeBuff,
			const std::size_t &writeSize);

	/**
	 * Get information about all registers and register files contained by current register file.
	 * @return Information string
	 */
	std::string getInfo() const;

	/**
	 * Get information about register or register file contained by current register file by name
	 * @param nameIn Name of register or register file
	 * @return Information string
	 */
	std::string getInfo(const std::string &nameIn) const;

	/**
	 * Get information about register or register file contained by current register file by address
	 * @param addrIn Address of register or register file
	 * @return Information string
	 */
	std::string getInfo(const ::hv::common::hvaddr_t &addrIn) const;

	/**
	 * Get information about all registers and register files contained by current register file (recursively).
	 * @return Information string
	 */
	std::string getRecursiveInfo(unsigned int level = 0) const;

	/**
	 * Get flattened info about all registers contained by current register file and its children,
	 * relatively to current RegisterFile
	 * @return Information string
	 */
	std::string getFlattenedInfo() const;

protected:
	/**
	 * Register file name
	 */
	const std::string name;

	/**
	 * Register file description
	 */
	const std::string description;

	/**
	 * Represents the gap in bytes between two successive accessible elements
	 */
	const std::size_t alignment;

	/**
	 * Map address/register
	 */
	rmap_t registers;

	/**
	 * Map address/registerFiles
	 */
	rfmap_t registerFiles;

	/**
	 * Map address/register listing all descending hierarchy
	 */
	rmap_t allRegisters;

	/**
	 * Addresses of registers to delete at RegisterFile destruction
	 */
	std::vector<Register*> regsToDelete;

	/**
	 * Addresses of RegisterFiles to delete at RegisterFile destruction
	 */
	std::vector<RegisterFile*> regFilesToDelete;

private:
	/**
	 * If fixedSize == 0, then this registerFile has no defined size.
	 * Its size is then virtually defined by the largest occupied address of its last register.
	 * Else, fixedSize is this registerFile's size in bytes.
	 * This variable is automatically set if current RegisterFile is integrated into another RegisterFile.
	 */
	std::size_t fixedSize;
}
;

} // namespace reg
} // namespace hv

#endif /* SRC_REGISTERFILE_H_ */
