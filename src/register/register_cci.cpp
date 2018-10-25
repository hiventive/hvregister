/**
 * @file register_cci.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register CCI compatibility class
 */

#include "register_cci.h"
#include "register.h"
#include "../cci/register_callback_if.h"

using namespace ::hv::common;

namespace hv {
namespace reg {

std::size_t RegisterCCI::getSize() const {
	return reg.getSize();
}

std::string RegisterCCI::getName() const {
	return reg.getName();
}

std::string RegisterCCI::getDescription() const {
	return reg.getDescription();
}

::cci::cci_value RegisterCCI::getCCIValue() const {
	return ::cci::cci_value(reg.getValue());
}

void RegisterCCI::setCCIValue(const ::cci::cci_value& val) {
	val.get<BitVector>();
}

void RegisterCCI::createField(const std::string &fieldName,
		const std::size_t &ind1, const std::size_t &ind2) {
	reg.createField(fieldName, ind1, ind2);
}

::cci::cci_value RegisterCCI::getFieldCCIValue(const std::string &fieldName) const {
	return ::cci::cci_value(reg(fieldName));
}

std::pair<std::size_t, std::size_t> RegisterCCI::getFieldIndexes(
		const std::string &name) const {
	std::size_t indLow, indHigh;
	hvrwmode_t mode;
	if (reg.fields.get(name, &indLow, &indHigh, &mode)) {
		HV_ERR("Field does not exist")
		exit(EXIT_FAILURE);
	}
	return std::pair<std::size_t, std::size_t>(indLow, indHigh);
}

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
::cci::cci_value RegisterCCI::getVectorSelectionCCIValue(const std::size_t &ind1,
		const std::size_t &ind2) const {
	return ::cci::cci_value(reg.data(ind1, ind2));
}

/**
 * Bit selection
 *
 * Is equivalent to vector selection with ind1 == ind2
 * @param ind Index of the bit to be selected
 * @return Bit value
 */
::cci::cci_value RegisterCCI::getBitSelectionCCIValue(const std::size_t &ind) const {
	return ::cci::cci_value(reg.data[ind]);
}

/**
 * Set raw value
 * @param vp Value address
 */
void RegisterCCI::setRawValue(const void *vp) {
	reg.write(*static_cast<const BitVector*>(vp));
}

/**
 * Get raw value
 * @return value address
 */
const void* RegisterCCI::getRawValue() const {
	BitVector tmp;
	reg.read(tmp);
	return reg.data.getDataAddress();
}

/**
 * Set raw read mask value
 * @param vp Value address
 */
void RegisterCCI::setRawReadMaskValue(const void *vp) {
	reg.setReadMask(*static_cast<const BitVector*>(vp));
}

/**
 * Get raw read mask value
 * @return Value address
 */
const void* RegisterCCI::getRawReadMaskValue() const {
	return reg.readMask.getDataAddress();
}

/**
 * Set raw write mask value
 * @param vp Value address
 */
void RegisterCCI::setRawWriteMaskValue(const void *vp) {
	reg.setWriteMask(*static_cast<const BitVector*>(vp));
}

/**
 * Get raw write mask value
 * @return Value address
 */
const void* RegisterCCI::getRawWriteMaskValue() const {
	return reg.writeMask.getDataAddress();
}

bool RegisterCCI::hasCallbacks() const {
	return (!preWriteCallbackVect.empty() || !postWriteCallbackVect.empty()
			|| !preReadCallbackVect.empty() || !postReadCallbackVect.empty());
}

CallbackUntypedHandle RegisterCCI::registerPreWriteCallback(
		const CallbackUntypedHandle& cb) {
	preWriteCallbackVect.push_back(cb);
	return cb;
}

bool RegisterCCI::unregisterPreWriteCallback(const CallbackUntypedHandle& cb) {
	for (auto it = preWriteCallbackVect.begin();
			it != preWriteCallbackVect.end(); ++it) {
		if (it->cb == cb.cb) {
			preWriteCallbackVect.erase(it);
			return true;
		}
	}
	return false;
}
CallbackUntypedHandle RegisterCCI::registerPostWriteCallback(
		const CallbackUntypedHandle& cb) {
	postWriteCallbackVect.push_back(cb);
	return cb;
}

bool RegisterCCI::unregisterPostWriteCallback(const CallbackUntypedHandle& cb) {
	for (auto it = postWriteCallbackVect.begin();
			it != postWriteCallbackVect.end(); ++it) {
		if (it->cb == cb.cb) {
			postWriteCallbackVect.erase(it);
			return true;
		}
	}
	return false;
}

CallbackUntypedHandle RegisterCCI::registerPreReadCallback(
		const CallbackUntypedHandle& cb) {
	preReadCallbackVect.push_back(cb);
	return cb;
}

bool RegisterCCI::unregisterPreReadCallback(const CallbackUntypedHandle& cb) {
	for (auto it = preReadCallbackVect.begin(); it != preReadCallbackVect.end();
			++it) {
		if (it->cb == cb.cb) {
			preReadCallbackVect.erase(it);
			return true;
		}
	}
	return false;
}

CallbackUntypedHandle RegisterCCI::registerPostReadCallback(
		const CallbackUntypedHandle& cb) {
	postReadCallbackVect.push_back(cb);
	return cb;
}

bool RegisterCCI::unregisterPostReadCallback(const CallbackUntypedHandle& cb) {
	for (auto it = postReadCallbackVect.begin();
			it != postReadCallbackVect.end(); ++it) {
		if (it->cb == cb.cb) {
			postReadCallbackVect.erase(it);
			return true;
		}
	}
	return false;
}

bool RegisterCCI::unregisterAllCallbacks() {
	preReadCallbackVect.clear();
	postReadCallbackVect.clear();
	preWriteCallbackVect.clear();
	postWriteCallbackVect.clear();
	return true;
}

bool RegisterCCI::runPreWriteCallbacks(
		const ::hv::hvcci::RegisterWriteEvent<>& ev) {
	for (auto it = this->preWriteCallbackVect.begin();
			it != this->preWriteCallbackVect.end(); ++it) {
		if (!::hv::common::CallbackTypedHandle<
				bool(const ::hv::hvcci::RegisterWriteEvent<>&)>(*it)(ev))
			return false;
	}
	return true;
}

void RegisterCCI::runPostWriteCallbacks(
		const ::hv::hvcci::RegisterWriteEvent<>& ev) {
	for (auto it = this->postWriteCallbackVect.begin(); it != this->postWriteCallbackVect.end(); ++it) {
		::hv::common::CallbackTypedHandle<void(const ::hv::hvcci::RegisterWriteEvent<>&)>(*it).invoke(ev);
	}
}

bool RegisterCCI::runPreReadCallbacks(
		const ::hv::hvcci::RegisterReadEvent<>& ev) {
	for (auto it = this->preReadCallbackVect.begin();
			it != this->preReadCallbackVect.end(); ++it) {
		if (!::hv::common::CallbackTypedHandle<
				bool(const ::hv::hvcci::RegisterReadEvent<>&)>(*it)(ev))
			return false;
	}
	return true;
}

void RegisterCCI::runPostReadCallbacks(
		const ::hv::hvcci::RegisterReadEvent<>& ev) {
	for (auto it = this->postReadCallbackVect.begin();
			it != this->postReadCallbackVect.end(); ++it) {
		::hv::common::CallbackTypedHandle<
				void(const ::hv::hvcci::RegisterReadEvent<>&)>(*it).invoke(ev);
	}
}

RegisterCCI::RegisterCCI(Register &regIn) :
		reg(regIn) {
}

} // namespace hv
} // namespace reg
