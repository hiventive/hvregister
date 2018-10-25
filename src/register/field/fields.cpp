/**
 * @file fields.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Field representation
 */

#include "fields.h"

using namespace ::hv::common;

namespace hv {
namespace reg {

Fields::Fields() {
}

Fields::Fields(const Fields& src) {
	for (fieldsmap_t::const_iterator it = fields.cbegin(); it != fields.cend();
			++it) {
		this->add(it->first, it->second);
	}
}

Fields::~Fields() {
}

void Fields::add(const std::string &name, const std::size_t &indLow,
		const std::size_t &indHigh, const std::string &description,
		const hvrwmode_t &mode) {
	if (indHigh < indLow) {
		HV_ERR("indHigh must be superior or equal to indLow");
	}
	if (recovers(indLow, indHigh)) {
		HV_WARN(
				"Field " + name
						+ " is being added on a space already covered by another field")
	}
	if (fields.find(name) != fields.cend()) {
		HV_ERR("Field " + name + " already exists")
		exit(EXIT_FAILURE);
	}
	if (!fields.insert(
			std::pair<std::string, Field>(name,
					Field(indLow, indHigh, description, mode))).second) {
		HV_ERR("Error in creation of field " + name);
		exit(EXIT_FAILURE);
	}
}

void Fields::add(const std::string& nameIn, const Field &src) {
	this->add(nameIn, src.getIndLow(), src.getIndHigh(), src.getDescription(), src.getRWMode());
}

Fields::fieldsmap_t::const_iterator Fields::find(const std::size_t indLowIn,
		std::size_t indHighIn) const {
	fieldsmap_t::const_iterator it;
	for (it = fields.cbegin(); it != fields.cend(); ++it) {
		if ((it->second.getIndLow() == indLowIn)
				&& (it->second.getIndHigh() == indHighIn)) {
			break;
		}
	}
	return it;
}

bool Fields::get(const std::string &name, std::size_t *indLow,
		std::size_t *indHigh, hvrwmode_t *mode) const {
	fieldsmap_t::const_iterator it = fields.find(name);
	if (it == fields.cend()) {
		HV_WARN("Field " + name + " does not exist");
		return false;
	}
	*indLow = it->second.getIndLow();
	*indHigh = it->second.getIndHigh();
	*mode = it->second.getRWMode();
	return true;
}

Fields::fieldsmap_t::const_iterator Fields::cbegin() const {
	return fields.cbegin();
}

Fields::fieldsmap_t::const_iterator Fields::cend() const {
	return fields.cend();
}

bool Fields::recovers(const std::size_t &indLow,
		const std::size_t &indHigh) const {
	for (fieldsmap_t::const_iterator it = fields.cbegin(); it != fields.cend();
			++it) {
		std::size_t indLowTmp(it->second.getIndLow());
		std::size_t indHighTmp(it->second.getIndHigh());
		if (!((indLow > indHighTmp) || (indHigh < indLowTmp))) {
			return true;
		}
	}
	return false;
}
std::vector<std::pair<std::string, Field>> Fields::getFieldsSortedByIndex(
		bool acceptRecovering) const {
	std::vector<std::pair<std::string, Field>> retVect;
	for (fieldsmap_t::const_iterator it = fields.cbegin(); it != fields.cend();
			++it) {
		retVect.push_back(std::pair<std::string, Field>(it->first, it->second));
	}

	std::sort(retVect.begin(), retVect.end(),
			[](const std::pair<std::string, Field> &op1, const std::pair<std::string, Field> &op2) {return op1.second.getIndLow() < op2.second.getIndLow();});
	if (!retVect.empty() && !acceptRecovering) {
		std::vector<std::pair<std::string, Field>>::iterator it =
				retVect.begin();
		std::size_t indHighSave = it->second.getIndHigh();
		++it;
		while (it != retVect.end()) {
			if (it->second.getIndLow() <= indHighSave) {
				it = retVect.erase(it);
			} else {
				++it;
			}
		}

	}
	return retVect;
}

}
}
