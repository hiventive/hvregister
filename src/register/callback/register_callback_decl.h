/**
 * @file register_callback.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Hiventive Register callback definitions class
 *
 * This file defines the events and typedefs for pre/post read/write callbacks
 */

#ifndef HV_REGISTER_CALLBACK_DECL_H_
#define HV_REGISTER_CALLBACK_DECL_H_

#include <hv/common.h>
#include <algorithm>

namespace hv {
namespace reg {

// Forward declaration of RegisterIf class
class RegisterIf;

struct RegisterReadEvent {

	RegisterReadEvent(const ::hv::common::BitVector& val, RegisterIf &reg);

	virtual ~RegisterReadEvent() {
	}

	/**
	 * Register value when first calling read()
	 */
	::hv::common::BitVector value;
	/**
	 * Register handle
	 */
	RegisterIf& rh;
};

struct RegisterWriteEvent {

	RegisterWriteEvent(const ::hv::common::BitVector& oldVal, const ::hv::common::BitVector& newVal,
			RegisterIf &reg);

	virtual ~RegisterWriteEvent() {
	}

	/**
	 * Register old and new values
	 */
	::hv::common::BitVector oldValue, newValue;
	/**
	 * Register handle
	 */
	RegisterIf& rh;
};

typedef ::hv::common::CallbackImpl<bool(const RegisterReadEvent&)> PreReadCallback;
typedef ::hv::common::CallbackImpl<void(const RegisterReadEvent&)> PostReadCallback;
typedef ::hv::common::CallbackImpl<bool(const RegisterWriteEvent&)> PreWriteCallback;
typedef ::hv::common::CallbackImpl<void(const RegisterWriteEvent&)> PostWriteCallback;

} // namespace reg
} // namespace hv

#endif /* HV_REGISTER_CALLBACK_DECL_H_ */
