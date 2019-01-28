/**
 * @file register.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register class
 */

#ifndef HV_REGISTER_H_
#define HV_REGISTER_H_

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <HVCommon>
#include "register/register_if.h"
#include "register/callback/register_callback_if.h"
#include "register/register_cci.h"
#include "register/field/fields.h"

namespace hv {
namespace reg {

/**
 * Register class
 */
class Register: public RegisterIf, public RegisterCallbackIf {
	friend class RegisterCCI;
public:
//** Type definitions **//
	typedef std::vector<PreReadCallback> PreReadCallbackVector;
	typedef std::vector<PostReadCallback> PostReadCallbackVector;
	typedef std::vector<PreWriteCallback> PreWriteCallbackVector;
	typedef std::vector<PostWriteCallback> PostWriteCallbackVector;

//** Constructors **//
	/**
	 * Register constructor
	 * @param sizeIn Register size in bit
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 * @param modeIn Read/Write mode
	 * @param resetIn Reset value
	 */
	Register(const std::size_t sizeIn, const std::string &nameIn,
			const std::string &descriptionIn = std::string(""),
			const ::hv::common::hvrwmode_t &modeIn =
					::hv::common::hvrwmode_t::NA,
			const ::hv::common::BitVector &resetIn = ::hv::common::BitVector(
					0u));

	/**
	 * Copy constructor
	 * @param src Source register for copy
	 */
	Register(const Register& src);

//** Destructor **//
	virtual ~Register();

//** Accessors **//
	/**
	 * Get register size in bits
	 * @return Register size
	 */
	std::size_t getSize() const override;

	/**
	 * Get minimal register size in bytes
	 * @return Register size
	 */
	std::size_t getSizeInBytes() const override;

	/**
	 * Get name
	 * @return Register name
	 */
	std::string getName() const override;

	/**
	 * Get description
	 * @return Register description
	 */
	std::string getDescription() const override;

	/**
	 * Get Read/Write mode
	 * @return Register Read/Write mode
	 */
	::hv::common::hvrwmode_t getRWMode() const override;

	/**
	 * Get read mask
	 * @return Register read mask
	 */
	::hv::common::BitVector getReadMask() const override;

	/**
	 * Get write mask
	 * @return Register write mask
	 */
	::hv::common::BitVector getWriteMask() const override;

	/**
	 * Get const register value
	 * @param readMasked Applies read mask to value if true
	 * @return Register unmasked (default) or masked value
	 */
	::hv::common::BitVector getValue(const bool &applyReadMask = false) const
			override;

//** Modifiers **//
	/**
	 * Set reset value
	 * @param resetIn Reset value
	 */
	void setResetValue(const ::hv::common::BitVector &resetIn);

	/**
	 * Set read mask
	 *
	 * This mask is applied when using method read().
	 * Warning: it is advised to use this method ONLY if
	 * you want to use a custom read mask. Otherwise, the mask
	 * is auto-resolved.
	 * @param readMaskVal Read mask value
	 */
	void setReadMask(const ::hv::common::BitVector &readMaskVal) override;

	/**
	 * Set write mask
	 *
	 * This mask is applied when using method write().
	 * Warning: it is advised to use this method ONLY if
	 * you want to use a custom read mask. Otherwise, the mask
	 * is auto-resolved.
	 * @param writeMaskVal Write mask value
	 */
	void setWriteMask(const ::hv::common::BitVector &writeMaskVal) override;

	/**
	 * Set unmasked (default) or masked value
	 * @param src Value to be set
	 * @param applyWriteMask Applies write mask to src if true
	 */
	void setValue(const ::hv::common::BitVector &src,
			const bool &applyWriteMask = false) override;

//** Reset **//
	/**
	 * Resets register value to resetVal
	 */
	void reset();

//** Read/Write **//
	/**
	 * Read data from register
	 * @param readBuff Read buffer
	 * @param readSize Read size in bytes
	 * @return true if success
	 */
	bool read(::hv::common::hvuint8_t* readBuff, const std::size_t &readSize)
			override;

	/**
	 * Write data to register
	 * @param writeBuff Write buffer
	 * @param writeSize Write size in bytes
	 * @return true if success
	 */
	bool write(const ::hv::common::hvuint8_t* writeBuff,
			const std::size_t &writeSize) override;

	/**
	 * Read from register and store in BitVector
	 * @param dest Destination BitVector address
	 * @return true if success
	 */
	bool read(::hv::common::BitVector& dest) override;

	/**
	 * Write to register from BitVector
	 * @param src Source BitVector for writing
	 * @return true if success
	 */
	bool write(const ::hv::common::BitVector& src) override;

//** Casts **//
	/**
	 * Cast to bool
	 */
	operator bool() const;

	/**
	 * Cast to hvuint8_t
	 */
	operator ::hv::common::hvuint8_t() const;

	/**
	 * Cast to hvuint16_t
	 */
	operator ::hv::common::hvuint16_t() const;

	/**
	 * Cast to hvuint32_t
	 */
	operator ::hv::common::hvuint32_t() const;

	/**
	 * Cast to hvuint64_t
	 */
	operator ::hv::common::hvuint64_t() const;

	/**
	 * Cast to hvint8_t
	 */
	operator ::hv::common::hvint8_t() const;

	/**
	 * Cast to hvint16_t
	 */
	operator ::hv::common::hvint16_t() const;

	/**
	 * Cast to hvint32_t
	 */
	operator ::hv::common::hvint32_t() const;

	/**
	 * Cast to hvint64_t
	 */
	operator ::hv::common::hvint64_t() const;

	/**
	 * Cast to std::string
	 */
	operator ::std::string() const;

//** Operator overloading **//
	// Assignment
	/**
	 * Assignment from bool
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const bool &src);

	/**
	 * Assignment from hvuint8_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvuint8_t &src);

	/**
	 * Assignment from hvuint16_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvuint16_t &src);

	/**
	 * Assignment from hvuint32_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvuint32_t &src);

	/**
	 * Assignment from hvuint64_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvuint64_t &src);

	/**
	 * Assignment from hvint8_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvint8_t &src);

	/**
	 * Assignment from hvint16_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvint16_t &src);

	/**
	 * Assignment from hvint32_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvint32_t &src);

	/**
	 * Assignment from hvint64_t
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::hvint64_t &src);

	/**
	 * Assignment from std::string
	 *
	 * The string must be composed of '0' and '1' characters.
	 * If HV_BV_STR_MSB_FIRST is defined then index 0 is the MSB,
	 * otherwise the LSB
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::std::string &src);

	/**
	 * Assignment from BitVector
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const ::hv::common::BitVector &src);

	/**
	 * Assignment from Register
	 * @param src Source for assignment
	 * @return Reference to this
	 */
	Register& operator =(const Register &src);

	// Shifting
	/**
	 * Left shift
	 * @param nShift Left shift depth
	 * @return Result of left shifting
	 */
	::hv::common::BitVector operator <<(
			const ::hv::common::hvuint32_t &nShift) const;

	/**
	 * Left shift
	 * @param nShift Left shift depth
	 * @return Result of left shifting
	 */
	::hv::common::BitVector operator <<(
			const ::hv::common::hvint32_t &nShift) const;

	/**
	 * Right shift
	 * @param nShift Right shift depth
	 * @return Result of right shifting
	 */
	::hv::common::BitVector operator >>(
			const ::hv::common::hvuint32_t &nShift) const;

	/**
	 * Right shift
	 * @param nShift Right shift depth
	 * @return Result of right shifting
	 */
	::hv::common::BitVector operator >>(
			const ::hv::common::hvint32_t &nShift) const;

	/**
	 * Self left shift
	 * @param nShift Self left shift depth
	 * @return Reference to this
	 */
	Register& operator <<=(const ::hv::common::hvuint32_t &nShift);

	/**
	 * Self left shift
	 * @param nShift Self left shift depth
	 * @return Reference to this
	 */
	Register& operator <<=(const ::hv::common::hvint32_t &nShift);

	/**
	 * Self right shift
	 * @param nShift Self right shift depth
	 * @return Reference to this
	 */
	Register& operator >>=(const ::hv::common::hvuint32_t &nShift);

	/**
	 * Self right shift
	 * @param nShift Self right shift depth
	 * @return Reference to this
	 */
	Register& operator >>=(const ::hv::common::hvint32_t &nShift);

	// Boolean tests
	/**
	 * Boolean negation
	 * @return True if Register == 0, false else
	 */
	bool operator !() const;

	/**
	 * Equal comparison
	 * @param op2 Right-hand operand
	 * @return True if equal, false else
	 */
	bool operator ==(const Register &op2) const;

	/**
	 * Different comparison
	 * @param op2 Right-hand operand
	 * @return True if different, false else
	 */
	bool operator !=(const Register &op2) const;

	/**
	 * Logical AND
	 * @param op2 Right-hand operand
	 * @return True if both operands are non-zero, false otherwise
	 */
	bool operator &&(const Register &op2) const;

	/**
	 * Logical OR
	 * @param op2 Right-hand operand
	 * @return True if at least one operand is non-zero, false otherwise
	 */
	bool operator ||(const Register &op2) const;

	// Binary operators
	/**
	 * Binary negation
	 * @return bit-negated data BitVector
	 */
	::hv::common::BitVector operator ~() const;

	/**
	 * Binary AND
	 * @param op2 Right-hand operand
	 * @return Binary AND result
	 */
	::hv::common::BitVector operator &(const Register &op2) const;

	/**
	 * Binary OR
	 * @param op2 Right-hand operand
	 * @return Binary OR result
	 */
	::hv::common::BitVector operator |(const Register &op2) const;

	/**
	 * Binary XOR
	 * @param op2 Right-hand operand
	 * @return Binary XOR result
	 */
	::hv::common::BitVector operator ^(const Register &op2) const;

	/**
	 * Binary AND and assignment
	 * @param op2 Right-hand operand
	 * @return Reference to this
	 */
	Register& operator &=(const Register &op2);

	/**
	 * Binary AND and assignment
	 * @param op2 Right-hand operand
	 * @return Reference to this
	 */
	Register& operator |=(const Register &op2);

	/**
	 * Binary AND and assignment
	 * @param op2 Right-hand operand
	 * @return Reference to this
	 */
	Register& operator ^=(const Register &op2);

	// Concatenation
	/**
	 * Concatenation operator
	 *
	 * Concatenates Registers as a single BitVector. Left-hand operand is set to MSB side,
	 * right-hand operand to LSB side.
	 * @param op2 Right-hand operand
	 * @return Concatenated BitVector
	 */
	::hv::common::BitVector operator +(const Register &op2) const;

	// Vector and bit selection
	/**
	 * Vector selection
	 *
	 * If reg is a register, then reg(ind1, ind2) selects the bits
	 * between indexes ind1 and ind2. the order of ind1 and ind2 is
	 * not important.
	 * @param ind1 First index (LSB, resp. MSB of selection)
	 * @param ind2 Second index (MSB, resp. LSB of selection)
	 * @return BitVector representing selected vector
	 */
	::hv::common::BitVector operator ()(const std::size_t &ind1,
			const std::size_t &ind2) override;

	/**
	 * Vector selection - const version
	 *
	 * @param ind1 First index (LSB, resp. MSB of selection)
	 * @param ind2 Second index (MSB, resp. LSB of selection)
	 * @return BitVector representing selected vector's value
	 */
	::hv::common::BitVector operator ()(const std::size_t &ind1,
			const std::size_t &ind2) const override;

	/**
	 * Bit selection
	 *
	 * Is equivalent to vector selection with ind1 == ind2
	 * @param ind Index of the bit to be selected
	 * @return BitVector representing selected bit
	 */
	::hv::common::BitVector operator [](const std::size_t &ind) override;

	/**
	 * Bit selection - const version
	 *
	 * Is equivalent to vector selection with ind1 == ind2
	 * @param ind Index of the bit to be selected
	 * @return BitVector representing selected bit's value
	 */
	::hv::common::BitVector operator [](const std::size_t &ind) const override;

//** Field manipulation **//
	/**
	 * Create field in register
	 * @param fieldName Field name
	 * @param ind1 Starting index (MSB, resp. LSB)
	 * @param ind2 Ending index (LSB, resp. MSB)
	 * @param fieldDescription Field description
	 * @param fieldRWMode Field Read/Write mode
	 */
	void createField(const std::string &fieldName, const std::size_t &ind1,
			const std::size_t &ind2, const std::string &fieldDescription,
			const ::hv::common::hvrwmode_t &fieldRWMode =
					::hv::common::hvrwmode_t::NA);

	/**
	 * Create one-bit field in register
	 * @param fieldName Field name
	 * @param ind Field index
	 * @param fieldDescription Field description
	 * @param fieldRWMode Field Read/Write mode
	 */
	void createField(const std::string &fieldName, const std::size_t &ind,
			const std::string &fieldDescription,
			const ::hv::common::hvrwmode_t &fieldRWMode =
					::hv::common::hvrwmode_t::NA);

	/**
	 * Create field in register without description
	 * @param fieldName Field name
	 * @param ind1 Starting index (MSB, resp. LSB)
	 * @param ind2 Ending index (LSB, resp. MSB)
	 * @param fieldRWMode Field Read/Write mode
	 */
	void createField(const std::string &fieldName, const std::size_t &ind1,
			const std::size_t &ind2,
			const ::hv::common::hvrwmode_t &fieldRWMode);

	/**
	 * Create one-bit field in register without description
	 * @param fieldName Field name
	 * @param ind Field index
	 * @param fieldRWMode Field Read/Write mode
	 */
	void createField(const std::string &fieldName, const std::size_t &ind,
			const ::hv::common::hvrwmode_t &fieldRWMode);

	/**
	 * Field creation
	 * @param fieldName Field name
	 * @param ind1 Field LSB (resp. MSB) index
	 * @param ind2 Field MSB (resp. LSB) index
	 */
	void createField(const std::string &fieldName, const std::size_t &ind1,
			const std::size_t &ind2) override;

	/**
	 * Get field value
	 * @param fieldName Field name
	 * @return BitVector representing field
	 */
	::hv::common::BitVector operator ()(const std::string &fieldName) override;

	/**
	 * Get field value - const version
	 * @param fieldName Field name
	 * @return BitVector representing field value
	 */
	::hv::common::BitVector operator ()(const std::string &fieldName) const
			override;

	/**
	 * Get the indexes delimiting a given field
	 * @param fieldName Field name
	 * @return A pair of indexes (LSB, MSB) of the desired field
	 */
	std::pair<std::size_t, std::size_t> getFieldIndexes(
			const std::string &fieldName) const override;

//** Callbacks **//
	// Hiventive callbacks
	std::size_t howManyPreReadCallbacks() const override;
	std::size_t howManyPostReadCallbacks() const override;
	std::size_t howManyPreWriteCallbacks() const override;
	std::size_t howManyPostWriteCallbacks() const override;
	std::size_t howManyCallbacks() const override;

	::hv::common::hvcbID_t registerPreReadCallback(const PreReadCallback &cb)
			override;
	::hv::common::hvcbID_t registerPostReadCallback(const PostReadCallback &cb)
			override;
	::hv::common::hvcbID_t registerPreWriteCallback(const PreWriteCallback &cb)
			override;
	::hv::common::hvcbID_t registerPostWriteCallback(
			const PostWriteCallback &cb) override;

	template<typename T> ::hv::common::hvcbID_t registerPreReadCallback(
			bool (T::*cb)(const RegisterReadEvent&), T *obj) {
		return this->registerPreReadCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPreReadCallback(
			bool (T::*cb)(const RegisterReadEvent&) const, T *obj) {
		return this->registerPreReadCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPostReadCallback(
			void (T::*cb)(const RegisterReadEvent&), T *obj) {
		return registerPostReadCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPostReadCallback(
			void (T::*cb)(const RegisterReadEvent&) const, T *obj) {
		return registerPostReadCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPreWriteCallback(
			bool (T::*cb)(const RegisterWriteEvent&), T *obj) {
		return registerPreWriteCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPreWriteCallback(
			bool (T::*cb)(const RegisterWriteEvent&) const, T *obj) {
		return registerPreWriteCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPostWriteCallback(
			void (T::*cb)(const RegisterWriteEvent&), T *obj) {
		return registerPostWriteCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	template<typename T> ::hv::common::hvcbID_t registerPostWriteCallback(
			void (T::*cb)(const RegisterWriteEvent&) const, T *obj) {
		return registerPostWriteCallback(
				std::bind(cb, obj, std::placeholders::_1));
	}

	bool unregisterPreReadCallback(const ::hv::common::hvcbID_t &id) override;
	bool unregisterPostReadCallback(const ::hv::common::hvcbID_t &id) override;
	bool unregisterPreWriteCallback(const ::hv::common::hvcbID_t &id) override;
	bool unregisterPostWriteCallback(const ::hv::common::hvcbID_t &id) override;
	bool unregisterAllCallbacks() override;
	// End Hiventive callbacks

	bool runPreReadCallbacks(const RegisterReadEvent& ev);
	void runPostReadCallbacks(const RegisterReadEvent& ev);
	bool runPreWriteCallbacks(const RegisterWriteEvent& ev);
	void runPostWriteCallbacks(const RegisterWriteEvent& ev);

private:
	PreReadCallbackVector::iterator getPreReadCallbackIt(
			const ::hv::common::hvcbID_t &id);

	PostReadCallbackVector::iterator getPostReadCallbackIt(
			const ::hv::common::hvcbID_t &id);

	PreWriteCallbackVector::iterator getPreWriteCallbackIt(
			const ::hv::common::hvcbID_t &id);

	PostWriteCallbackVector::iterator getPostWriteCallbackIt(
			const ::hv::common::hvcbID_t &id);

public:
//** Info display **//
	/**
	 * Displays information about Register
	 *
	 * Warning: if there are recovering fields (which is allowed) only
	 * one amongst all recovering fields will be displayed in
	 * schematics. However all will appear in fields list and details
	 * @return Info string
	 */
	std::string getInfo(::hv::common::hvuint32_t level = 1u) const;
//** Helpers **//
	/**
	 * Register data value to string
	 * @return String representing binary register value (default: MSB first)
	 */
	std::string toString() const;

	/**
	 * Output stream operator overloading
	 * @param strm Stream
	 * @param vs Binary vector object to output
	 * @return Stream
	 */
	friend std::ostream& operator <<(std::ostream &strm, const Register &reg);

protected:
//** Private helpers **//
	/**
	 * Updates read and write masks from register and fields modes
	 */
	void updateMasks();

	/**
	 * Get user-friendly table representing register value and fields
	 *
	 * Warning: if there are recovering fields (which is allowed) only
	 * one amongst all recovering fields will be displayed
	 * Note: this function has public access using getInfo(0)
	 * @return Info string
	 */
	std::string getRegTable() const;

	/**
	 * Get a unique ID
	 * @return Unique ID
	 */
	::hv::common::hvcbID_t getUniqueID();

//** Member values **//
	/**
	 * Register name
	 */
	const std::string name;

	/**
	 * Register description
	 */
	const std::string description;

	/**
	 * Register read/write mode
	 */
	const ::hv::common::hvrwmode_t mode;

	/**
	 * Register data
	 */
	::hv::common::BitVector data;

	/**
	 * Register reset value
	 */
	::hv::common::BitVector resetVal;

	/**
	 * Register read mask
	 */
	::hv::common::BitVector readMask;

	/**
	 * Register writeMask;
	 */
	::hv::common::BitVector writeMask;

	/**
	 * Register fields
	 */
	Fields fields;

	/**
	 * Read and write locks
	 */
	bool readLock, writeLock;

	/**
	 * Pre-read callback vector
	 */
	PreReadCallbackVector preReadCbVect;

	/**
	 * Post-read callback vector
	 */
	PostReadCallbackVector postReadCbVect;

	/**
	 * Pre-write callback vector
	 */
	PreWriteCallbackVector preWriteCbVect;

	/**
	 * Post-write callback vector
	 */
	PostWriteCallbackVector postWriteCbVect;

	/**
	 * Callback ID counter
	 */
	::hv::common::hvcbID_t cbIDCpt;

private:
	RegisterCCI regCCI;
};

// Interoperability

// OPERATOR ==
template<typename T> inline bool reg_operator_isequal1(const Register &a,
		const T &b) {
	return (a.getValue() == ::hv::common::BitVector(BITWIDTH_OF(T), b));
}

template<typename T> inline bool reg_operator_isequal2(const T &a,
		const Register &b) {
	return (b == a);
}

#define REG_OP_ISEQUAL(T) inline bool operator == (const Register &a, const T &b) {return reg_operator_isequal1(a, b);} \
inline bool operator == (const T &a, const Register &b) {return reg_operator_isequal2(a, b);}
REG_OP_ISEQUAL(::hv::common::hvuint8_t)
REG_OP_ISEQUAL(::hv::common::hvuint16_t)
REG_OP_ISEQUAL(::hv::common::hvuint32_t)
REG_OP_ISEQUAL(::hv::common::hvuint64_t)
REG_OP_ISEQUAL(::hv::common::hvint8_t)
REG_OP_ISEQUAL(::hv::common::hvint16_t)
REG_OP_ISEQUAL(::hv::common::hvint32_t)
REG_OP_ISEQUAL(::hv::common::hvint64_t)
REG_OP_ISEQUAL(std::string)
REG_OP_ISEQUAL(::hv::common::BitVector)

// OPERATOR !=
template<typename T> inline bool reg_operator_isdifferent1(const Register &a,
		const T &b) {
	return (a.getValue() != ::hv::common::BitVector(b));
}

template<typename T> inline bool reg_operator_isdifferent2(const T &a,
		const Register &b) {
	return (b != a);
}

#define REG_OP_ISDIFFERENT(T) inline bool operator != (const Register &a, const T &b) {return reg_operator_isdifferent1(a, b);} \
inline bool operator != (const T &a, const Register &b) {return reg_operator_isdifferent2(a, b);}
REG_OP_ISDIFFERENT(::hv::common::hvuint8_t)
REG_OP_ISDIFFERENT(::hv::common::hvuint16_t)
REG_OP_ISDIFFERENT(::hv::common::hvuint32_t)
REG_OP_ISDIFFERENT(::hv::common::hvuint64_t)
REG_OP_ISDIFFERENT(::hv::common::hvint8_t)
REG_OP_ISDIFFERENT(::hv::common::hvint16_t)
REG_OP_ISDIFFERENT(::hv::common::hvint32_t)
REG_OP_ISDIFFERENT(::hv::common::hvint64_t)
REG_OP_ISDIFFERENT(std::string)
REG_OP_ISDIFFERENT(::hv::common::BitVector)

// OPERATOR &&
template<typename T> inline bool reg_operator_booland1(const Register &a,
		const T &b) {
	return (a.getValue() && ::hv::common::BitVector(b));
}

template<typename T> inline bool reg_operator_booland2(const T &a,
		const Register &b) {
	return (b && a);
}

#define REG_OP_BOOLAND(T) inline bool operator && (const Register &a, const T &b) {return reg_operator_booland1(a, b);} \
inline bool operator && (const T &a, const Register &b) {return reg_operator_booland2(a, b);}
REG_OP_BOOLAND(bool)
REG_OP_BOOLAND(::hv::common::hvuint8_t)
REG_OP_BOOLAND(::hv::common::hvuint16_t)
REG_OP_BOOLAND(::hv::common::hvuint32_t)
REG_OP_BOOLAND(::hv::common::hvuint64_t)
REG_OP_BOOLAND(::hv::common::hvint8_t)
REG_OP_BOOLAND(::hv::common::hvint16_t)
REG_OP_BOOLAND(::hv::common::hvint32_t)
REG_OP_BOOLAND(::hv::common::hvint64_t)
REG_OP_BOOLAND(std::string)
REG_OP_BOOLAND(::hv::common::BitVector)

// OPERATOR ||
template<typename T> inline bool reg_operator_boolor1(const Register &a,
		const T &b) {
	return (a.getValue() || ::hv::common::BitVector(b));
}

template<typename T> inline bool reg_operator_boolor2(const T &a,
		const Register &b) {
	return (b || a);
}

#define REG_OP_BOOLOR(T) inline bool operator || (const Register &a, const T &b) {return reg_operator_boolor1(a, b);} \
inline bool operator || (const T &a, const Register &b) {return reg_operator_boolor2(a, b);}
REG_OP_BOOLOR(bool)
REG_OP_BOOLOR(::hv::common::hvuint8_t)
REG_OP_BOOLOR(::hv::common::hvuint16_t)
REG_OP_BOOLOR(::hv::common::hvuint32_t)
REG_OP_BOOLOR(::hv::common::hvuint64_t)
REG_OP_BOOLOR(::hv::common::hvint8_t)
REG_OP_BOOLOR(::hv::common::hvint16_t)
REG_OP_BOOLOR(::hv::common::hvint32_t)
REG_OP_BOOLOR(::hv::common::hvint64_t)
REG_OP_BOOLOR(std::string)
REG_OP_BOOLOR(::hv::common::BitVector)

// OPERATOR &
template<typename T> inline ::hv::common::BitVector reg_operator_bitwiseand1(
		const Register &a, const T &b) {
	return (a.getValue()
			& ::hv::common::BitVector(HV_MIN(a.getSize(), BITWIDTH_OF(T)), b));
}

template<typename T> inline ::hv::common::BitVector reg_operator_bitwiseand2(
		const T &a, const Register &b) {
	return (b & a);
}

#define REG_OP_BITWISEAND(T) inline ::hv::common::BitVector operator & (const Register &a, const T &b) {return reg_operator_bitwiseand1(a, b);} \
inline ::hv::common::BitVector operator & (const T &a, const Register &b) {return reg_operator_bitwiseand2(a, b);}
REG_OP_BITWISEAND(::hv::common::hvuint8_t)
REG_OP_BITWISEAND(::hv::common::hvuint16_t)
REG_OP_BITWISEAND(::hv::common::hvuint32_t)
REG_OP_BITWISEAND(::hv::common::hvuint64_t)
REG_OP_BITWISEAND(::hv::common::hvint8_t)
REG_OP_BITWISEAND(::hv::common::hvint16_t)
REG_OP_BITWISEAND(::hv::common::hvint32_t)
REG_OP_BITWISEAND(::hv::common::hvint64_t)
REG_OP_BITWISEAND(std::string)
REG_OP_BITWISEAND(::hv::common::BitVector)

// OPERATOR |
template<typename T> inline ::hv::common::BitVector reg_operator_bitwiseor1(
		const Register &a, const T &b) {
	return (a.getValue() | ::hv::common::BitVector(b));
}

template<typename T> inline ::hv::common::BitVector reg_operator_bitwiseor2(
		const T &a, const Register &b) {
	return (b | a);
}

#define REG_OP_BITWISEOR(T) inline ::hv::common::BitVector operator | (const Register &a, const T &b) {return reg_operator_bitwiseor1(a, b);} \
inline ::hv::common::BitVector operator | (const T &a, const Register &b) {return reg_operator_bitwiseor2(a, b);}
REG_OP_BITWISEOR(::hv::common::hvuint8_t)
REG_OP_BITWISEOR(::hv::common::hvuint16_t)
REG_OP_BITWISEOR(::hv::common::hvuint32_t)
REG_OP_BITWISEOR(::hv::common::hvuint64_t)
REG_OP_BITWISEOR(::hv::common::hvint8_t)
REG_OP_BITWISEOR(::hv::common::hvint16_t)
REG_OP_BITWISEOR(::hv::common::hvint32_t)
REG_OP_BITWISEOR(::hv::common::hvint64_t)
REG_OP_BITWISEOR(std::string)
REG_OP_BITWISEOR(::hv::common::BitVector)

// OPERATOR ^
template<typename T> inline ::hv::common::BitVector reg_operator_bitwisexor1(
		const Register &a, const T &b) {
	return (a.getValue() ^ ::hv::common::BitVector(b));
}

template<typename T> inline ::hv::common::BitVector reg_operator_bitwisexor2(
		const T &a, const Register &b) {
	return (b ^ a);
}

#define REG_OP_BITWISEXOR(T) inline ::hv::common::BitVector operator ^ (const Register &a, const T &b) {return reg_operator_bitwisexor1(a, b);} \
inline ::hv::common::BitVector operator ^ (const T &a, const Register &b) {return reg_operator_bitwisexor2(a, b);}
REG_OP_BITWISEXOR(::hv::common::hvuint8_t)
REG_OP_BITWISEXOR(::hv::common::hvuint16_t)
REG_OP_BITWISEXOR(::hv::common::hvuint32_t)
REG_OP_BITWISEXOR(::hv::common::hvuint64_t)
REG_OP_BITWISEXOR(::hv::common::hvint8_t)
REG_OP_BITWISEXOR(::hv::common::hvint16_t)
REG_OP_BITWISEXOR(::hv::common::hvint32_t)
REG_OP_BITWISEXOR(::hv::common::hvint64_t)
REG_OP_BITWISEXOR(std::string)
REG_OP_BITWISEXOR(::hv::common::BitVector)

// Operator &=
// Only one, the other one is achieved by implicit conversion
template<typename T> inline T& reg_operator_bitwiseandequal(T &a,
		const Register &b) {
	return (a &= T(b.getValue()));
}
#define REG_OP_BITWISEANDEQUAL(T) inline T& operator &= (T &a, const Register &b) {return reg_operator_bitwiseandequal(a, b);}
REG_OP_BITWISEANDEQUAL(::hv::common::hvuint8_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvuint16_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvuint32_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvuint64_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvint8_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvint16_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvint32_t)
REG_OP_BITWISEANDEQUAL(::hv::common::hvint64_t)
// Specialization for std::string
inline std::string& operator &=(std::string &a, const Register &b) {
	::hv::common::BitVector tmp(a);
	tmp &= b.getValue();
	a = tmp.toString();
	return a;
}
REG_OP_BITWISEANDEQUAL(::hv::common::BitVector)

// Operator |=
// Only one, the other one is achieved by implicit conversion
template<typename T> inline T& reg_operator_bitwiseorequal(T &a,
		const Register &b) {
	return (a |= T(b.getValue()));
}
#define REG_OP_BITWISEOREQUAL(T) inline T& operator |= (T &a, const Register &b) {return reg_operator_bitwiseorequal(a, b);}
REG_OP_BITWISEOREQUAL(::hv::common::hvuint8_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvuint16_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvuint32_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvuint64_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvint8_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvint16_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvint32_t)
REG_OP_BITWISEOREQUAL(::hv::common::hvint64_t)
// Specialization for std::string
inline std::string& operator |=(std::string &a, const Register &b) {
	::hv::common::BitVector tmp(a);
	tmp |= b.getValue();
	a = tmp.toString();
	return a;
}
REG_OP_BITWISEOREQUAL(::hv::common::BitVector)

// Operator ^=
// Only one, the other one is achieved by implicit conversion
template<typename T> inline T& reg_operator_bitwisexorequal(T &a,
		const Register &b) {
	return (a ^= T(b.getValue()));
}
#define REG_OP_BITWISEXOREQUAL(T) inline T& operator ^= (T &a, const Register &b) {return reg_operator_bitwisexorequal(a, b);}
REG_OP_BITWISEXOREQUAL(::hv::common::hvuint8_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvuint16_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvuint32_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvuint64_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvint8_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvint16_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvint32_t)
REG_OP_BITWISEXOREQUAL(::hv::common::hvint64_t)
// Specialization for std::string
inline std::string& operator ^=(std::string &a, const Register &b) {
	::hv::common::BitVector tmp(a);
	tmp ^= b.getValue();
	a = tmp.toString();
	return a;
}
REG_OP_BITWISEXOREQUAL(::hv::common::BitVector)

// Operator +
template<typename T> inline ::hv::common::BitVector reg_operator_concat1(
		const Register &a, const T &b) {
	return (a.getValue() + ::hv::common::BitVector(b));
}

template<typename T> inline ::hv::common::BitVector reg_operator_concat2(
		const T& a, const Register &b) {
	return (::hv::common::BitVector(a) + b.getValue());
}

#define REG_OP_PLUS(T) inline ::hv::common::BitVector operator +(const Register &a, const T &b) { return reg_operator_concat1(a, b); } \
inline ::hv::common::BitVector operator +(const T &a, const Register &b) { return reg_operator_concat2(a, b); }
REG_OP_PLUS(::hv::common::hvuint8_t)
REG_OP_PLUS(::hv::common::hvuint16_t)
REG_OP_PLUS(::hv::common::hvuint32_t)
REG_OP_PLUS(::hv::common::hvuint64_t)
REG_OP_PLUS(::hv::common::hvint8_t)
REG_OP_PLUS(::hv::common::hvint16_t)
REG_OP_PLUS(::hv::common::hvint32_t)
REG_OP_PLUS(::hv::common::hvint64_t)
REG_OP_PLUS(std::string)
REG_OP_PLUS(::hv::common::BitVector)

}
}

#endif /* HV_REGISTER_H_ */
