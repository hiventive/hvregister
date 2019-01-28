/**
 * @file register_callback.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Hiventive Register callback class
 */

#include "register/callback/register_callback_decl.h"


namespace hv {
namespace reg {

RegisterReadEvent::RegisterReadEvent(const ::hv::common::BitVector& val, RegisterIf &reg) :
		value(val), rh(reg) {
}

RegisterWriteEvent::RegisterWriteEvent(const ::hv::common::BitVector& oldVal,
		const ::hv::common::BitVector& newVal, RegisterIf &reg) :
		oldValue(oldVal), newValue(newVal), rh(reg) {
}
} // namespace reg
} // namespace hv

