/**
 * @file register_if.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register interface class for CCI
 */

#ifndef CCI_REGISTER_IF_H_
#define CCI_REGISTER_IF_H_

#include <HVConfiguration>
#include "register/field/field_if.h"
#include "cci/register_callback_if.h"

namespace hv {
namespace hvcci {

class RegisterIf: public RegisterCallbackIf {
	friend class RegisterUntypedHandle;
public:
//** Destructor **//
	/**
	 * Destructor
	 */
	virtual ~RegisterIf() {
	}

//** Accessors **//

	/**
	 * Get size in bits
	 * @return Size in bits
	 */
	virtual std::size_t getSize() const = 0;

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
	 * Get cci value
	 * @return cci_value
	 */
	virtual ::cci::cci_value getCCIValue() const = 0;

//** Modifiers **//
	/**
	 * Set cci value
	 * @param val Value to be set
	 */
	virtual void setCCIValue(const ::cci::cci_value& val) = 0;

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
	 * Get field value
	 * @param name Field name
	 * @return cci_value representing field's accessor or value
	 */
	virtual ::cci::cci_value getFieldCCIValue(const std::string &fieldName) const = 0;

	/**
	 * Get field indexes from field name
	 * @param fieldName Field name
	 * @return An std::pair containing the lowest and highest index of the field
	 */
	virtual std::pair<std::size_t, std::size_t> getFieldIndexes(
			const std::string &fieldName) const = 0;

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
	virtual ::cci::cci_value getVectorSelectionCCIValue(const std::size_t &ind1,
			const std::size_t &ind2) const = 0;

	/**
	 * Bit selection
	 *
	 * Is equivalent to vector selection with ind1 == ind2
	 * @param ind Index of the bit to be selected
	 * @return Bit value
	 */
	virtual ::cci::cci_value getBitSelectionCCIValue(const std::size_t &ind) const = 0;

protected:
	/**
	 * Set raw value
	 * @param vp Value address
	 */
	virtual void setRawValue(const void *vp) = 0;

	/**
	 * Get raw value
	 * @return value address
	 */
	virtual const void* getRawValue() const = 0;

	/**
	 * Set raw read mask value
	 * @param vp Value address
	 */
	virtual void setRawReadMaskValue(const void *vp) = 0;

	/**
	 * Get raw read mask value
	 * @return Value address
	 */
	virtual const void* getRawReadMaskValue() const = 0;

	/**
	 * Set raw write mask value
	 * @param vp Value address
	 */
	virtual void setRawWriteMaskValue(const void *vp) = 0;

	/**
	 * Get raw write mask value
	 * @return Value address
	 */
	virtual const void* getRawWriteMaskValue() const = 0;
};

} // namespace cci
} // namespace hv

#endif /* CCI_REGISTER_IF_H_ */
