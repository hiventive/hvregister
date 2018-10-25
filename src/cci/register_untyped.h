/*
 * RegisterUntyped.h
 *
 *  Created on: Jul 15, 2018
 *      Author: bbarrois
 */

#ifndef SRC_CCI_REGISTER_UNTYPED_H_
#define SRC_CCI_REGISTER_UNTYPED_H_

#include <hv/common.h>
#include <algorithm>

#include "register_if.h"

namespace hv {
namespace hvcci {

class RegisterUntyped: public RegisterIf {
public:

	/**
	 * Constructor
	 * @param sizeIn Register size
	 * @param nameIn Register name
	 * @param descriptionIn Register description
	 */
	// TODO Add broker + originator
	RegisterUntyped(std::size_t sizeIn, std::string nameIn,
			std::string descriptionIn);

	~RegisterUntyped() {

	}

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

protected:
	std::size_t mSize;
	std::string mName;
	std::string mDescription;

	/**
	 * Callback object - gathers callback, originator and any other tracking data (TODO)
	 */
	template<class T>
	struct CallbackObj {
		CallbackObj(T cb) :
				callback(cb) {
		}
		T callback;
		// TODO add originator
	};

	/**
	 * Callback object Vector
	 */
	template<class T>
	struct CallbackObjVector {
		CallbackObjVector() {
		}
		std::vector<T> vec;
	};

	//TODO Register untyped shall contain handles

	/// Pre write callbacks
	typedef CallbackObj<typename ::hv::common::CallbackUntypedHandle::type> pre_write_callback_obj_t;

	CallbackObjVector<pre_write_callback_obj_t> mPreWriteCallbacks;

	/// Post write callbacks
	typedef CallbackObj<typename ::hv::common::CallbackUntypedHandle::type> post_write_callback_obj_t;

	CallbackObjVector<post_write_callback_obj_t> mPostWriteCallbacks;

	/// Pre read callbacks
	typedef CallbackObj<typename ::hv::common::CallbackUntypedHandle::type> pre_read_callback_obj_t;

	CallbackObjVector<pre_read_callback_obj_t> mPreReadCallbacks;

	/// Post read callbacks
	typedef CallbackObj<typename ::hv::common::CallbackUntypedHandle::type> post_read_callback_obj_t;

	CallbackObjVector<post_read_callback_obj_t> mPostReadCallbacks;

};

}
}

#endif /* SRC_CCI_REGISTER_UNTYPED_H_ */
