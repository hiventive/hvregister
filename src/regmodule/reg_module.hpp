/**
 * @file reg_module.hpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date May, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register-style module
 */


namespace hv {
namespace reg {

template<unsigned int BUSWIDTH> RegModule<BUSWIDTH>::RegModule(::hv::module::ModuleName name_, const std::size_t &alignment) :
		::hv::module::Module(name_), memMapSocket(
				"MemMapSocket"), mainRegisterFile(name_ + "_mainRegFile",
				"Main Register File of " + std::string(name_), alignment) {
	memMapSocket.registerBTransport(this, &RegModule<BUSWIDTH>::bTransportCb);
}

template<unsigned int BUSWIDTH> RegModule<BUSWIDTH>::~RegModule() {
}

template<unsigned int BUSWIDTH> RegisterFile& RegModule<BUSWIDTH>::getMainRegFile() {
	return mainRegisterFile;
}

template<unsigned int BUSWIDTH> bool RegModule<BUSWIDTH>::addRegister(::hv::common::hvaddr_t address, Register& src) {
	return mainRegisterFile.addRegister(address, src);
}

template<unsigned int BUSWIDTH> bool RegModule<BUSWIDTH>::addRegister(Register& src) {
	return mainRegisterFile.addRegister(src);
}

template<unsigned int BUSWIDTH> bool RegModule<BUSWIDTH>::addRegisterFile(::hv::common::hvaddr_t address, RegisterFile& src) {
	return mainRegisterFile.addRegisterFile(address, src);
}

template<unsigned int BUSWIDTH> bool RegModule<BUSWIDTH>::addRegisterFile(RegisterFile& src) {
	return mainRegisterFile.addRegisterFile(src);
}

template<unsigned int BUSWIDTH> bool RegModule<BUSWIDTH>::addRegModule(::hv::common::hvaddr_t address, RegModule& src) {
	return mainRegisterFile.addRegisterFile(address, src.mainRegisterFile);
}

template<unsigned int BUSWIDTH> bool RegModule<BUSWIDTH>::addRegModule(RegModule& src) {
	return mainRegisterFile.addRegisterFile(src.mainRegisterFile);
}

template<unsigned int BUSWIDTH> std::string RegModule<BUSWIDTH>::getInfo() const {
	return mainRegisterFile.getRecursiveInfo();
}

template<unsigned int BUSWIDTH> void RegModule<BUSWIDTH>::bTransportCb(mem_access_payload_type& txn,
		::sc_core::sc_time& delay) {
	::hv::common::hvaddr_t destination = txn.getAddress();
	::hv::communication::tlm2::protocols::memorymapped::MemoryMappedCommand cmd(
			txn.getCommand());
	std::size_t txnSize(txn.getDataLength());

	// Transaction size can be larger than only one register
	// Therefore we concatenate the values of subsequent registers in our buffer
	while (txnSize) {
		Register& regTmp(mainRegisterFile(destination));
		std::size_t opSize(HV_MIN(txnSize, regTmp.getSizeInBytes()));
		::hv::common::hvuint8_t* dataTmp(txn.getDataPtr());
		if (cmd
				== ::hv::communication::tlm2::protocols::memorymapped::MEM_MAP_READ_COMMAND) {
			if (!regTmp.read(dataTmp, opSize)) {
				txn.setResponseStatus(
						::hv::communication::tlm2::protocols::memorymapped::MEM_MAP_GENERIC_ERROR_RESPONSE);
				return;
			}
		} else {
			if (!regTmp.write(dataTmp, opSize)) {
				txn.setResponseStatus(
						::hv::communication::tlm2::protocols::memorymapped::MEM_MAP_GENERIC_ERROR_RESPONSE);
				return;
			}
		}

		if (txnSize <= regTmp.getSizeInBytes()) {
			txnSize = 0u;
		} else {
			txnSize -= regTmp.getSizeInBytes();
			dataTmp += regTmp.getSizeInBytes() * sizeof(::hv::common::hvuint8_t);
			destination += regTmp.getSizeInBytes() * sizeof(::hv::common::hvuint8_t);
		}
	}
}

} // namespace hv
} // namespace module

