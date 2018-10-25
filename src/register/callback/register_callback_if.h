/**
 * @file register_callback_if.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register interface class for callback registration/unregistration
 */


#ifndef HV_REGISTER_CALLBACK_IF_H_
#define HV_REGISTER_CALLBACK_IF_H_

#include "register_callback_decl.h"

namespace hv {
namespace reg {

class RegisterCallbackIf {
public:
	virtual ~RegisterCallbackIf() {
	}

	virtual std::size_t howManyPreReadCallbacks() const = 0;
	virtual std::size_t howManyPostReadCallbacks() const = 0;
	virtual std::size_t howManyPreWriteCallbacks() const = 0;
	virtual std::size_t howManyPostWriteCallbacks() const = 0;
	virtual std::size_t howManyCallbacks() const = 0;

	virtual ::hv::common::hvcbID_t registerPreReadCallback(const PreReadCallback &cb) = 0;
	virtual ::hv::common::hvcbID_t registerPostReadCallback(const PostReadCallback &cb) = 0;
	virtual ::hv::common::hvcbID_t registerPreWriteCallback(const PreWriteCallback &cb) = 0;
	virtual ::hv::common::hvcbID_t registerPostWriteCallback(const PostWriteCallback &cb) = 0;

	virtual bool unregisterPreReadCallback(const ::hv::common::hvcbID_t &id) = 0;
	virtual bool unregisterPostReadCallback(const ::hv::common::hvcbID_t &id) = 0;
	virtual bool unregisterPreWriteCallback(const ::hv::common::hvcbID_t &id) = 0;
	virtual bool unregisterPostWriteCallback(const ::hv::common::hvcbID_t &id) = 0;

	virtual bool unregisterAllCallbacks() = 0;
};

}
}

#endif /* REGISTER_CALLBACK_IF_H_ */
