/**
 * @file register_if.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register interface class
 */

#ifndef HV_REGISTER_IF_H_
#define HV_REGISTER_IF_H_

#include <HVCommon>
#include "register/field/field_if.h"

namespace hv {
namespace reg {

class RegisterIf {
public:
//** Destructor **//
	/**
	 * Destructor
	 */
	virtual ~RegisterIf() {
	}

//** Accessors **//
	/**
	 * Get register size in bits
	 * @return Register size
	 */
	virtual std::size_t getSize() const = 0;

	/**
	 * Get minimal register size in bytes
	 * @return Register size
	 */
	virtual std::size_t getSizeInBytes() const = 0;

	/**
	 * Get name
	 * @return Register name
	 */
	virtual std::string getName() const = 0;

	/**
	 * Get description
	 * @return Register description
	 */
	virtual std::string getDescription() const = 0;

	/**
	 * Get Read/Write mode
	 * @return Register Read/Write mode
	 */
	virtual ::hv::common::hvrwmode_t getRWMode() const = 0;

	/**
	 * Get read mask
	 * @return Read mask
	 */
	virtual ::hv::common::BitVector getReadMask() const = 0;

	/**
	 * Get write mask
	 * @return Write mask
	 */
	virtual ::hv::common::BitVector getWriteMask() const = 0;

	/**
	 * Get const register value
	 * @param readMasked Applies read mask to value if true
	 * @return Register unmasked (default) or masked value
	 */
	virtual ::hv::common::BitVector getValue(const bool &applyReadMask = false) const = 0;

//** Modifiers **//
	/**
	 * Set read mask
	 * @param readMaskVal Read mask value
	 */
	virtual void setReadMask(const ::hv::common::BitVector &readMaskVal) = 0;

	/**
	 * Set write mask
	 * @param writeMaskVal Write mask value
	 */
	virtual void setWriteMask(const ::hv::common::BitVector &writeMaskVal) = 0;

	/**
	 * Set unmasked (default) or masked value
	 * @param src Value to be set
	 * @param applyWriteMask Applies write mask to src if true
	 */
	virtual void setValue(const ::hv::common::BitVector &src, const bool &applyWriteMask =
			false) = 0;

//** Handle getters **//
	// These functions return a handler to the desired sub-vectors of register
	// Therefore they can be used as a lvalue
	// e.g. reg(8, 5) = reg2(4, 1); reg[12] = 1;
	/**
	 * Returns a handle to register value between bit ind1 and ind2
	 * @param ind1 LSB (resp. MSB) of the selection
	 * @param ind2 MSB (resp. LSB) of the selection
	 * @return Accessor to register's data between ind1 and ind2
	 */
	virtual ::hv::common::BitVector operator()(const std::size_t &ind1,
			const std::size_t &ind2) = 0;

	/**
	 * Returns the value of register between bit ind1 and ind2 (read-only)
	 * @param ind1 LSB (resp. MSB) of the selection
	 * @param ind2 MSB (resp. LSB) of the selection
	 * @return Register's data value between ind1 and ind2
	 */
	virtual ::hv::common::BitVector operator()(const std::size_t &ind1,
			const std::size_t &ind2) const = 0;

	/**
	 * Returns a handle to register bit of index ind
	 * @param ind Selected index
	 * @return Accessor to register's data between ind1 and ind2
	 */
	virtual ::hv::common::BitVector operator[](const std::size_t &ind) = 0;

	/**
	 * Returns the value of register between bit ind1 and ind2 (read-only)
	 * @param ind LSB (resp. MSB) of the selection
	 * @return Register's data value between ind1 and ind2
	 */
	virtual ::hv::common::BitVector operator[](const std::size_t &ind) const = 0;

//** Read/Write **//
	/**
	 * Read data from register
	 * @param readBuff Read buffer
	 * @param readSize Read size in bytes
	 * @return true if success
	 */
	virtual bool read(::hv::common::hvuint8_t* readBuff, const std::size_t &readSize) = 0;

	/**
	 * Write data to register
	 * @param writeBuff Write buffer
	 * @param writeSize Write size in bytes
	 * @return true if success
	 */
	virtual bool write(const ::hv::common::hvuint8_t* writeBuff,
			const std::size_t &writeSize) = 0;

	/**
	 * Read from register and store in BitVector
	 * @param dest Destination BitVector address
	 * @return true if success
	 */
	virtual bool read(::hv::common::BitVector& dest) = 0;

	/**
	 * Write to register from BitVector
	 * @param src Source BitVector for writing
	 * @return true if success
	 */
	virtual bool write(const ::hv::common::BitVector& src) = 0;

//** Field creation/access **//
	/**
	 * Field creation
	 * @param fieldName Field name
	 * @param ind1 Field LSB (resp. MSB) index
	 * @param ind2 Field MSB (resp. LSB) index
	 */
	virtual void createField(const std::string &fieldName,
			const std::size_t &ind1, const std::size_t &ind2) = 0;

	/**
	 * Get handle to field
	 * @param fieldName Field name to get handle for
	 * @return Handle to field
	 */
	virtual ::hv::common::BitVector operator()(const std::string &fieldName) = 0;

	/**
	 * Get field value
	 * @param fieldName Field name to get value of
	 * @return Field's value
	 */
	virtual ::hv::common::BitVector operator()(const std::string &fieldName) const = 0;

	/**
	 * Get the indexes delimiting a given field
	 * @param fieldName Field name
	 * @return A pair of indexes (LSB, MSB) of the desired field
	 */
	virtual std::pair<std::size_t, std::size_t> getFieldIndexes(
			const std::string &fieldName) const = 0;

};

} // namespace reg
} // namespace hv

#endif /* HV_REGISTER_IF_H_ */
