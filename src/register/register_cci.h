/**
 * @file register_cci.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register CCI compatibility class
 */

#ifndef HV_REGISTER_CCI_H_
#define HV_REGISTER_CCI_H_

#include <HVCommon>
#include <HVConfiguration>
#include "cci/register_if.h"

namespace hv {
namespace reg {

// Register forward declarations
class Register;

class RegisterCCI: public ::hv::hvcci::RegisterIf {
	friend Register;
public:
	/**
	 * Get size in bits
	 * @return Size in bits
	 */
	std::size_t getSize() const override;

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
	 * Get cci value
	 * @return ::cci::cci_value
	 */
	::cci::cci_value getCCIValue() const override;

	/**
	 * Set cci value
	 * @param val Value to be set
	 */
	void setCCIValue(const ::cci::cci_value& val) override;

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
	 * @param name Field name
	 * @return ::cci::cci_value representing field's accessor or value
	 */
	::cci::cci_value getFieldCCIValue(const std::string &fieldName) const override;

	/**
	 * Get field indexes from field name
	 * @param name Field name
	 * @return An std::pair containing the lowest and highest index of the field
	 */
	std::pair<std::size_t, std::size_t> getFieldIndexes(
			const std::string &name) const override;

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
			const std::size_t &ind2) const override;

	/**
	 * Bit selection
	 *
	 * Is equivalent to vector selection with ind1 == ind2
	 * @param ind Index of the bit to be selected
	 * @return Bit value
	 */
	::cci::cci_value getBitSelectionCCIValue(const std::size_t &ind) const override;

	/**
	 * Set raw value
	 * @param vp Value address
	 */
	void setRawValue(const void *vp) override;

	/**
	 * Get raw value
	 * @return value address
	 */
	const void* getRawValue() const override;

	/**
	 * Set raw read mask value
	 * @param vp Value address
	 */
	void setRawReadMaskValue(const void *vp) override;

	/**
	 * Get raw read mask value
	 * @return Value address
	 */
	const void* getRawReadMaskValue() const override;

	/**
	 * Set raw write mask value
	 * @param vp Value address
	 */
	void setRawWriteMaskValue(const void *vp) override;

	/**
	 * Get raw write mask value
	 * @return Value address
	 */
	const void* getRawWriteMaskValue() const override;

	bool hasCallbacks() const override;
	::hv::common::CallbackUntypedHandle
	registerPreWriteCallback(const ::hv::common::CallbackUntypedHandle& cb) override;
	bool
	unregisterPreWriteCallback(const ::hv::common::CallbackUntypedHandle& cb) override;

	::hv::common::CallbackUntypedHandle
	registerPostWriteCallback(const ::hv::common::CallbackUntypedHandle& cb) override;
	bool
	unregisterPostWriteCallback(const ::hv::common::CallbackUntypedHandle& cb) override;

	::hv::common::CallbackUntypedHandle
	registerPreReadCallback(const ::hv::common::CallbackUntypedHandle& cb) override;
	bool
	unregisterPreReadCallback(const ::hv::common::CallbackUntypedHandle& cb) override;

	::hv::common::CallbackUntypedHandle
	registerPostReadCallback(const ::hv::common::CallbackUntypedHandle& cb) override;
	bool
	unregisterPostReadCallback(const ::hv::common::CallbackUntypedHandle& cb) override;

	bool unregisterAllCallbacks() override;

	bool runPreWriteCallbacks(const ::hv::hvcci::RegisterWriteEvent<>& ev);

	void runPostWriteCallbacks(const ::hv::hvcci::RegisterWriteEvent<>& ev);

	bool runPreReadCallbacks(const ::hv::hvcci::RegisterReadEvent<>& ev);

	void runPostReadCallbacks(const ::hv::hvcci::RegisterReadEvent<>& ev);

protected:
	/**
	 * Constructor from register
	 * @param regIn Attached register
	 */
	RegisterCCI(Register &regIn);

	virtual ~RegisterCCI() {
	}

//** Members **//

	/**
	 * Reference to attached register
	 */
	Register &reg;

	/**
	 * Pre-write callback vector
	 */
	typedef ::hv::common::CallbackTypedHandle<bool(const ::hv::hvcci::RegisterWriteEvent<>&)> preWriteCallback_t;
	std::vector<::hv::common::CallbackUntypedHandle> preWriteCallbackVect;

	/**
	 * Post-write callback vector
	 */
	typedef ::hv::common::CallbackTypedHandle<void(const ::hv::hvcci::RegisterWriteEvent<>&)> postWriteCallback_t;
	std::vector<::hv::common::CallbackUntypedHandle> postWriteCallbackVect;

	/**
	 * Pre-read callback vector
	 */
	typedef ::hv::common::CallbackTypedHandle<bool(const ::hv::hvcci::RegisterReadEvent<>&)> preReadCallback_t;
	std::vector<::hv::common::CallbackUntypedHandle> preReadCallbackVect;

	/**
	 * Post-read callback vector
	 */
	typedef ::hv::common::CallbackTypedHandle<void(const ::hv::hvcci::RegisterReadEvent<>&)> postReadCallback_t;
	std::vector<::hv::common::CallbackUntypedHandle> postReadCallbackVect;
};

} // namespace reg
} // namespace hv

#endif /* HV_REGISTER_CCI_H_ */

