/**
 * @file fields.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Field representation
 */

#include "field.h"

using namespace ::hv::common;

namespace hv {
namespace reg {

Field::Field(const std::size_t &indLowIn, const std::size_t &indHighIn,
		const std::string &descriptionIn, const hvrwmode_t &modeIn) :
		indLow(indLowIn), indHigh(indHighIn), description(descriptionIn), mode(
				modeIn) {
}

Field::Field(const Field &src) :
		indLow(src.indLow), indHigh(src.indHigh), description(src.description), mode(
				src.mode) {
}

Field::~Field() {
}

std::size_t Field::getIndLow() const {
	return indLow;
}

std::size_t Field::getIndHigh() const {
	return indHigh;
}

std::string Field::getDescription() const {
	return description;
}

hvrwmode_t Field::getRWMode() const {
	return mode;
}

} // namespace reg
} // namespace hv
