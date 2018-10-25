/**
 * @file register_untyped_handle.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief CCI Register callback untyped handle
 */

#include "register_untyped_handle.h"
#include "register_if.h"

using namespace ::cci;

namespace hv {
namespace hvcci {

RegisterUntypedHandle::RegisterUntypedHandle(RegisterIf& regIn) :
		mRegister(&regIn) {
}

RegisterUntypedHandle::RegisterUntypedHandle(RegisterUntypedHandle& regUHIn) :
		mRegister(regUHIn.mRegister) {

}

std::size_t RegisterUntypedHandle::getSize() const {
	return mRegister->getSize();
}

std::string RegisterUntypedHandle::getName() const {
	return mRegister->getName();
}

std::string RegisterUntypedHandle::getDescription() const {
	return mRegister->getDescription();
}

::cci::cci_value RegisterUntypedHandle::getCCIValue() const {
	return mRegister->getCCIValue();
}

void RegisterUntypedHandle::setCCIValue(const ::cci::cci_value& val) {
	mRegister->setCCIValue(val);
}

void RegisterUntypedHandle::createField(const std::string &fieldName,
		const std::size_t &ind1, const std::size_t &ind2) {
	mRegister->createField(fieldName, ind1, ind2);

}

::cci::cci_value RegisterUntypedHandle::getFieldCCIValue(
		const std::string &fieldName) const {
	return mRegister->getFieldCCIValue(fieldName);
}

std::pair<std::size_t, std::size_t> RegisterUntypedHandle::getFieldIndexes(
		const std::string &fieldName) const {
	return mRegister->getFieldIndexes(fieldName);
}

::cci::cci_value RegisterUntypedHandle::getVectorSelectionCCIValue(
		const std::size_t &ind1, const std::size_t &ind2) const {
	return mRegister->getVectorSelectionCCIValue(ind1, ind2);
}

::cci::cci_value RegisterUntypedHandle::getBitSelectionCCIValue(
		const std::size_t &ind) const {
	return mRegister->getBitSelectionCCIValue(ind);
}

bool RegisterUntypedHandle::hasCallbacks() const {
	return mRegister->hasCallbacks();
}

::hv::common::CallbackUntypedHandle RegisterUntypedHandle::registerPreWriteCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->registerPreWriteCallback(cb);
}

bool RegisterUntypedHandle::unregisterPreWriteCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->unregisterPreWriteCallback(cb);
}

::hv::common::CallbackUntypedHandle RegisterUntypedHandle::registerPostWriteCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->registerPostWriteCallback(cb);
}

bool RegisterUntypedHandle::unregisterPostWriteCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->unregisterPostWriteCallback(cb);
}

::hv::common::CallbackUntypedHandle RegisterUntypedHandle::registerPreReadCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->registerPreReadCallback(cb);
}

bool RegisterUntypedHandle::unregisterPreReadCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->unregisterPreReadCallback(cb);
}

::hv::common::CallbackUntypedHandle RegisterUntypedHandle::registerPostReadCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->registerPostReadCallback(cb);
}

bool RegisterUntypedHandle::unregisterPostReadCallback(
		const ::hv::common::CallbackUntypedHandle& cb) {
	return mRegister->unregisterPostReadCallback(cb);
}

bool RegisterUntypedHandle::unregisterAllCallbacks() {
	return mRegister->unregisterAllCallbacks();
}

void RegisterUntypedHandle::setRawValue(const void *vp) {
	mRegister->setRawValue(vp);
}

const void* RegisterUntypedHandle::getRawValue() const {
	return mRegister->getRawValue();
}

void RegisterUntypedHandle::setRawReadMaskValue(const void *vp) {
	mRegister->setRawReadMaskValue(vp);
}

const void* RegisterUntypedHandle::getRawReadMaskValue() const {
	return mRegister->getRawReadMaskValue();
}

void RegisterUntypedHandle::setRawWriteMaskValue(const void *vp) {
	mRegister->setRawWriteMaskValue(vp);
}

const void* RegisterUntypedHandle::getRawWriteMaskValue() const {
	return mRegister->getRawWriteMaskValue();
}

}
}

