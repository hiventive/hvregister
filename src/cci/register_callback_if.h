/**
 * @file register_callback_if.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief CCI Register callback interface
 */

#ifndef CCI_REGISTER_CALLBACK_IF_H_
#define CCI_REGISTER_CALLBACK_IF_H_

#include <hv/common.h>

#include "register_untyped_handle.h"

namespace hv {
namespace hvcci {

/**
 * API interface for callbacks registration/unregistration
 */
struct RegisterCallbackIf {
	virtual ~RegisterCallbackIf() {
	}

	virtual bool hasCallbacks() const = 0;
protected:
	virtual ::hv::common::CallbackUntypedHandle registerPreWriteCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;
	virtual bool unregisterPreWriteCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;

	virtual ::hv::common::CallbackUntypedHandle registerPostWriteCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;
	virtual bool unregisterPostWriteCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;

	virtual ::hv::common::CallbackUntypedHandle registerPreReadCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;
	virtual bool unregisterPreReadCallback(const ::hv::common::CallbackUntypedHandle& cb) = 0;

	virtual ::hv::common::CallbackUntypedHandle registerPostReadCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;
	virtual bool unregisterPostReadCallback(
			const ::hv::common::CallbackUntypedHandle& cb) = 0;

	virtual bool unregisterAllCallbacks() = 0;
};

//** Register read/write event definitions and implementation **//

template<typename T = void> struct RegisterWriteEvent;

template<> struct RegisterWriteEvent<void> {
	typedef RegisterWriteEvent type;
	typedef ::cci::cci_value value_type;

	RegisterWriteEvent(const value_type& oldValueIn,
			const value_type& newValueIn,
			const RegisterUntypedHandle& regHandleIn);
	virtual ~RegisterWriteEvent() {
	}

	const value_type& oldValue;
	const value_type& newValue;
	const RegisterUntypedHandle &regHandle;
};

template<typename T> struct RegisterWriteEvent {
	typedef RegisterWriteEvent type;
	typedef T value_type;

	RegisterWriteEvent(const value_type& oldValueIn,
			const value_type& newValueIn,
			const RegisterUntypedHandle& regHandleIn);
	virtual ~RegisterWriteEvent() {
	}

	const value_type& oldValue;
	const value_type& newValue;
	const RegisterUntypedHandle &regHandle;
};

template<typename T = void> struct RegisterReadEvent;

template<> struct RegisterReadEvent<void> {
	typedef RegisterReadEvent type;
	typedef ::cci::cci_value value_type;

	RegisterReadEvent(const value_type& valueIn,
			const RegisterUntypedHandle& regHandleIn);
	virtual ~RegisterReadEvent() {
	}

	const value_type& value;
	const RegisterUntypedHandle &regHandle;
};

template<typename T> struct RegisterReadEvent {
	typedef RegisterReadEvent type;
	typedef T value_type;

	RegisterReadEvent(const value_type& valueIn,
			const RegisterUntypedHandle& regHandleIn);
	virtual ~RegisterReadEvent() {
	}

	const value_type& value;
	const RegisterUntypedHandle &regHandle;
};

inline RegisterWriteEvent<void>::RegisterWriteEvent(
		const value_type& oldValueIn, const value_type& newValueIn,
		const RegisterUntypedHandle& regHandleIn) :
		oldValue(oldValueIn), newValue(newValueIn), regHandle(regHandleIn) {

}

template<typename T> RegisterWriteEvent<T>::RegisterWriteEvent(
		const value_type& oldValueIn, const value_type& newValueIn,
		const RegisterUntypedHandle& regHandleIn) :
		oldValue(oldValueIn), newValue(newValueIn), regHandle(regHandleIn) {

}

inline RegisterReadEvent<void>::RegisterReadEvent(
		const value_type& valueIn, const RegisterUntypedHandle& regHandleIn) :
		value(valueIn), regHandle(regHandleIn) {

}

template<typename T> RegisterReadEvent<T>::RegisterReadEvent(
		const value_type& valueIn, const RegisterUntypedHandle& regHandleIn) :
		value(valueIn), regHandle(regHandleIn) {

}

//** Pre/post read/write callbacks/callbacks handles definition **//

#ifdef HV_CPLUSPLUS_NO_ALIAS_TEMPLATE

template<typename T = void> struct RegisterPreWriteCallback : ::hv::common::CallbackImpl<bool(const RegisterWriteEvent<T>&)> {};
template<typename T = void> struct RegisterPreWriteCallbackHandle : ::hv::common::CallbackTypedHandle<bool(const RegisterWriteEvent<T>&)> {};
template<typename T = void> struct RegisterPostWriteCallback : ::hv::common::CallbackImpl<void(const RegisterWriteEvent<T>&)> {};
template<typename T = void> struct RegisterPostWriteCallbackHandle : ::hv::common::CallbackTypedHandle<void(const RegisterWriteEvent<T>&)> {};
template<typename T = void> struct RegisterPreReadCallback : ::hv::common::CallbackImpl<bool(const RegisterReadEvent<T>&)> {};
template<typename T = void> struct RegisterPreReadCallbackHandle : ::hv::common::CallbackTypedHandle<bool(const RegisterReadEvent<T>&)> {};
template<typename T = void> struct RegisterPostReadCallback : ::hv::common::CallbackImpl<void(const RegisterReadEvent<T>&)> {};
template<typename T = void> struct RegisterPostReadCallbackHandle : ::hv::common::CallbackTypedHandle<void(const RegisterReadEvent<T>&)> {};

#else

template<typename T = void> using RegisterPreWriteCallback = ::hv::common::CallbackImpl<bool(const RegisterWriteEvent<T>&)>;
template<typename T = void> using RegisterPreWriteCallbackHandle = ::hv::common::CallbackTypedHandle<bool(const RegisterWriteEvent<T>&)>;
template<typename T = void> using RegisterPostWriteCallback = ::hv::common::CallbackImpl<void(const RegisterWriteEvent<T>&)>;
template<typename T = void> using RegisterPostWriteCallbackHandle = ::hv::common::CallbackTypedHandle<void(const RegisterWriteEvent<T>&)>;
template<typename T = void> using RegisterPreReadCallback = ::hv::common::CallbackImpl<bool(const RegisterReadEvent<T>&)>;
template<typename T = void> using RegisterPreReadCallbackHandle = ::hv::common::CallbackTypedHandle<bool(const RegisterReadEvent<T>&)>;
template<typename T = void> using RegisterPostReadCallback = ::hv::common::CallbackImpl<void(const RegisterReadEvent<T>&)>;
template<typename T = void> using RegisterPostReadCallbackHandle = ::hv::common::CallbackTypedHandle<void(const RegisterReadEvent<T>&)>;

#endif
//** Read/write events/callbacks untyped definition **//
typedef RegisterWriteEvent<>::type RegisterUntypedWriteEvent;
typedef RegisterPreWriteCallback<>::type RegisterPreWriteCallbackUntyped;
typedef RegisterPostWriteCallback<>::type RegisterPostWriteCallbackUntyped;

typedef RegisterReadEvent<>::type RegisterReadWriteEvent;
typedef RegisterPreReadCallback<>::type RegisterPreReadCallbackUntyped;
typedef RegisterPostReadCallback<>::type RegisterPostReadCallbackUntyped;

}// namespace hvcci
} // namespace hv

#endif

