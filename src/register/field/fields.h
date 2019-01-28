/**
 * @file fields.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Fields mapping
 */

#ifndef HV_FIELDS_H_
#define HV_FIELDS_H_

#include <algorithm>
#include <vector>
#include <map>
#include <sstream>
#include <HVCommon>
#include "register/field/field.h"

namespace hv {
namespace reg {

class Fields {
public:
	/**
	 * Type definition for fields map
	 */
	typedef std::map<std::string, Field> fieldsmap_t;

	/**
	 * Default constructor
	 *
	 * Fields are empty
	 */
	Fields();

	/**
	 * Copy constructor
	 * @param src Source for copy
	 */
	Fields(const Fields& src);

	/**
	 * Destructor
	 */
	virtual ~Fields();

	/**
	 * Add field
	 * @param name Field name
	 * @param indLow Lowest index
	 * @param indHigh Highest index
	 * @param description Field description
	 * @param mode Read/Write, Read-Only or Write-Only mode
	 */
	void add(const std::string &name, const std::size_t &indLow,
			const std::size_t &indHigh, const std::string &description,
			const ::hv::common::hvrwmode_t &mode);

	/** Add field
	 * @param nameIn Field name
	 * @param src Field to add
	 */
	void add(const std::string& nameIn, const Field &src);

	/**
	 * Find field from indexes
	 * If field does not exist, returns fields.cend()
	 *
	 * @param indLowIn Potential field low index
	 * @param indHighIn Potential field high index
	 * @return True if field exists, false otherwise
	 */
	fieldsmap_t::const_iterator find(const std::size_t indLowIn, std::size_t indHighIn) const;

	/**
	 * Get field by name
	 * @param name Name
	 * @param indLow Address of return variable for lowest index
	 * @param indHigh Address of return variable for highest index
	 * @param mode Address of return variable for mode
	 * @return true if found, false otherwise
	 */
	bool get(const std::string &name, std::size_t* indLow, std::size_t* indHigh,
			::hv::common::hvrwmode_t *mode) const;

	/**
	 * Get beginning of fields map
	 * @return const iterator pointing to fields map beginning
	 */
	fieldsmap_t::const_iterator cbegin() const;

	/**
	 * Get end of fields map
	 * @return const iterator pointing to fields map end
	 */
	fieldsmap_t::const_iterator cend() const;

	/**
	 * Get a vector of current fields sorted by index
	 * @return Vector of current fields sorted by index
	 */
	std::vector<std::pair<std::string, Field>> getFieldsSortedByIndex(bool acceptRecovering = false) const;

protected:
	/**
	 * Checks if a new field recovers with existing fields
	 * @param indLow Lowest index of new field
	 * @param indHigh Highest index of new field
	 * @return true if field recovers, false otherwise
	 */
	bool recovers(const std::size_t &indLow, const std::size_t &indHigh) const;

	/**
	 * Fields map
	 */
	fieldsmap_t fields;
};

}
}

#endif /* HV_FIELDS_H_ */
