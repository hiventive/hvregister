/**
 * @file register_untyped_handle.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief CCI Register callback untyped handle
 */

#ifndef CCI_REGISTER_UNTYPED_HANDLE_H_
#define CCI_REGISTER_UNTYPED_HANDLE_H_

#include <HVCommon>
#include <HVConfiguration>

namespace hv {
namespace hvcci {

// Forward declaration
class RegisterIf;

class RegisterUntypedHandle {
public:
	RegisterUntypedHandle(RegisterIf& regIn);

	RegisterUntypedHandle(RegisterUntypedHandle& regUHIn);

	virtual ~RegisterUntypedHandle() {
	}
	//** Accessors **//

	/**
	 * Get size in bits
	 * @return Size in bits
	 */
	std::size_t getSize() const;

	/**
	 * Get name
	 * @return Register name
	 */
	std::string getName() const;

	/**
	 * Get description
	 * @return Register description
	 */
	std::string getDescription() const;

	/**
	 * Get cci value
	 * @return cci_value
	 */
	::cci::cci_value getCCIValue() const;

	//** Modifiers **//
	/**
	 * Set cci value
	 * @param val Value to be set
	 */
	void setCCIValue(const ::cci::cci_value& val);

	//** Field creation/access **//
	/**
	 * Field creation
	 * @param fieldName Field name
	 * @param ind1 Field LSB (resp. MSB) index
	 * @param ind2 Field MSB (resp. LSB) index
	 */
	void createField(const std::string &fieldName, const std::size_t &ind1,
			const std::size_t &ind2);

	/**
	 * Get field value
	 * @param name Field name
	 * @return cci_value representing field's accessor or value
	 */
	::cci::cci_value getFieldCCIValue(const std::string &fieldName) const;

	/**
	 * Get field indexes from field name
	 * @param fieldName Field name
	 * @return An std::pair containing the lowest and highest index of the field
	 */
	std::pair<std::size_t, std::size_t> getFieldIndexes(
			const std::string &fieldName) const;

	//** Vector/Bit selection **//
	/**
	 * Vector selection
	 *
	 * If reg is a register, then reg(ind1, ind2) selects the bits
	 * between indexes ind1 and ind2. the order of ind1 and ind2 is
	 * not important.
	 * @param ind1 First index (LSB, resp. MSB of selection)
	 * @param ind2 Second index (MSB, resp. LSB of selection)
	 * @return Vector value
	 */
	::cci::cci_value getVectorSelectionCCIValue(const std::size_t &ind1,
			const std::size_t &ind2) const;

	/**
	 * Bit selection
	 *
	 * Is equivalent to vector selection with ind1 == ind2
	 * @param ind Index of the bit to be selected
	 * @return Bit value
	 */
	::cci::cci_value getBitSelectionCCIValue(const std::size_t &ind) const;

	bool hasCallbacks() const;

	::hv::common::CallbackUntypedHandle registerPreWriteCallback(
			const ::hv::common::CallbackUntypedHandle& cb);

	bool unregisterPreWriteCallback(const ::hv::common::CallbackUntypedHandle& cb);

	::hv::common::CallbackUntypedHandle registerPostWriteCallback(
			const ::hv::common::CallbackUntypedHandle& cb);

	bool unregisterPostWriteCallback(const ::hv::common::CallbackUntypedHandle& cb);

	::hv::common::CallbackUntypedHandle registerPreReadCallback(
			const ::hv::common::CallbackUntypedHandle& cb);

	bool unregisterPreReadCallback(const ::hv::common::CallbackUntypedHandle& cb);

	::hv::common::CallbackUntypedHandle registerPostReadCallback(
			const ::hv::common::CallbackUntypedHandle& cb);

	bool
	unregisterPostReadCallback(const ::hv::common::CallbackUntypedHandle& cb);

	bool unregisterAllCallbacks();

protected:
	/**
	 * Set raw value
	 * @param vp Value address
	 */
	void setRawValue(const void *vp);

	/**
	 * Get raw value
	 * @return value address
	 */
	const void* getRawValue() const;

	/**
	 * Set raw read mask value
	 * @param vp Value address
	 */
	void setRawReadMaskValue(const void *vp);

	/**
	 * Get raw read mask value
	 * @return Value address
	 */
	const void* getRawReadMaskValue() const;

	/**
	 * Set raw write mask value
	 * @param vp Value address
	 */
	void setRawWriteMaskValue(const void *vp);

	/**
	 * Get raw write mask value
	 * @return Value address
	 */
	const void* getRawWriteMaskValue() const;

private:
	RegisterIf* mRegister;
};

}
}

#endif
