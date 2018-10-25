/**
 * @file field_if.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Field interface
 */

#ifndef HV_FIELD_IF_H_
#define HV_FIELD_IF_H_

#include <iostream>

namespace hv {
namespace reg {

/**
 * Class FieldIf
 *
 * indHigh and indLow must be set at construction
 */
class FieldIf {
public:
	/**
	 * Destructor
	 */
	virtual ~FieldIf() = 0;

	/**
	 * Get low index
	 * @return Low index
	 */
	virtual std::size_t getIndLow() const = 0;

	/**
	 * Get high index
	 * @return High index
	 */
	virtual std::size_t getIndHigh() const = 0;
};

} // namespace reg
} // namespace hv

#endif /* HV_FIELD_IF_H_ */
