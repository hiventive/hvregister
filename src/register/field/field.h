/**
 * @file field.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Field class
 */

#ifndef HV_FIELD_H_
#define HV_FIELD_H_

#include <iostream>
#include <hv/common.h>

#include "field_if.h"

namespace hv {
namespace reg {

class Field {
public:
	/**
	 * Field constructor
	 * @param indLowIn Lowest index of field
	 * @param indHighIn Highest index of field (>= indLowIn)
	 * @param descriptionIn Field description
	 * @param modeIn Read/Write, Read-Only or Write-Only mode
	 */
	Field(const std::size_t &indLowIn, const std::size_t &indHighIn,
			const std::string &descriptionIn, const ::hv::common::hvrwmode_t &modeIn);
	/**
	 * Copy constructor
	 * @param src Source field
	 */
	Field(const Field &src);

	/**
	 * Destructor
	 */
	virtual ~Field();

	/**
	 * Get low index
	 * @return Low index
	 */
	std::size_t getIndLow() const;

	/**
	 * Get high index
	 * @return High index
	 */
	std::size_t getIndHigh() const;

	/**
	 * Get description
	 * @return Description as a string
	 */
	std::string getDescription() const;

	/**
	 * Get field-level Read/Write mode
	 * @return
	 */
	::hv::common::hvrwmode_t getRWMode() const;

protected:
	std::size_t indLow;
	std::size_t indHigh;
	std::string description;
	::hv::common::hvrwmode_t mode;
};

} // namespace reg
} // namespace hv

#endif /* HV_FIELD_H_ */
