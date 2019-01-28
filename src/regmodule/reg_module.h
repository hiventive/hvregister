/**
 * @file reg_module.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date May, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register-style module
 */

#ifndef HV_REGMODULE_H
#define HV_REGMODULE_H

#include <HVCommunication>
#include <HVModule>
#include "registerfile/registerfile.h"

namespace hv {
namespace reg {

/**
 * RegModule class
 *
 * This class represents a register-based module
 * It contains a main register file mainRF
 */
template<unsigned int BUSWIDTH = 32> class RegModule: public ::hv::module::Module {
public:
	typedef ::hv::communication::tlm2::protocols::memorymapped::MemoryMappedProtocolTypes::tlm_payload_type mem_access_payload_type;

	/**
	 * RegModule constructor with name and optional alignment specification
	 * @param name Name of RegModule
	 * @param alignment Alignment value in bytes
	 */
	RegModule(::hv::module::ModuleName name_, const ::std::size_t &alignment = 0);
	virtual ~RegModule();

	/**
	 * Get reference to main register file
	 * @return Reference to main register file
	 */
	::hv::reg::RegisterFile& getMainRegFile();

	/**
	 * Add register to specified address
	 * @param address Insertion address
	 * @param src Register to insert
	 * @return true if success, false else
	 */
	bool addRegister(::hv::common::hvaddr_t address, ::hv::reg::Register& src);

	/**
	 * Add register with no specified address
	 * Register is inserted following alignment rules of main register file.
	 * @param src Register to insert
	 * @return true if success, false else
	 */
	bool addRegister(::hv::reg::Register& src);

	/**
	 * Add register file to specified address
	 * @param address Insertion address
	 * @param src Register file to insert
	 * @return true if success, false else
	 */
	bool addRegisterFile(::hv::common::hvaddr_t address,
			::hv::reg::RegisterFile& src);

	/**
	 * Add register file with no specified address
	 * Register file is inserted following alignment rules of main register file.
	 * @param src Register file to insert
	 * @return true if success, false else
	 */
	bool addRegisterFile(::hv::reg::RegisterFile& src);

	/**
	 * Add RegModule to specified address
	 * This method is equivalent to inserting src's main register file to
	 * current RegModule's main register file
	 * @param address Insertion address
	 * @param src Module to insert
	 * @return true if success, false else
	 */
	bool addRegModule(::hv::common::hvaddr_t address, RegModule& src);

	/**
	 * Add RegModule with no address specified
	 * This method is equivalent to inserting src's main register file to
	 * current RegModule's main register file
	 * Register file is inserted following alignment rules of main register file.
	 * @param src Module to insert
	 * @return true if success, false else
	 */
	bool addRegModule(RegModule& src);

	/**
	 * Get main register file info
	 * @return Main register file info
	 */
	::std::string getInfo() const;

	::hv::communication::tlm2::protocols::memorymapped::MemoryMappedSimpleTargetSocket<BUSWIDTH, ::hv::communication::tlm2::protocols::memorymapped::MemoryMappedProtocolTypes, 0> memMapSocket;

protected:
	void bTransportCb(mem_access_payload_type& txn, ::sc_core::sc_time& delay);

	::hv::reg::RegisterFile mainRegisterFile;
};

}
}

#include "regmodule/reg_module.hpp"

#endif // HV_REGMODULE_H
