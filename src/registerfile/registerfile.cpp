/**
 * @file registerfile.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jul, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief RegisterFile class
 */

#include "registerfile/registerfile.h"

using namespace ::hv::common;

namespace hv {
namespace reg {

RegisterFile::RegisterFile(std::string nameIn, std::string descriptionIn,
		std::size_t alignmentIn) :
		name(nameIn), description(descriptionIn), alignment(alignmentIn), fixedSize(
				0) {
	if ((alignment != std::size_t(0))
			&& (alignmentIn != superiorPowerOf2(alignmentIn))) {
		HV_ERR("Alignment must be a power of 2")
		exit(EXIT_FAILURE);
	}
}

RegisterFile::RegisterFile(const RegisterFile &src) :
		name(src.name), description(src.description), alignment(src.alignment), fixedSize(
				src.fixedSize) {
	// Copying registers
	for (rmap_t::const_iterator it = src.registers.cbegin();
			it != src.registers.cend(); ++it) {
		Register* regTmp = new Register(it->second);
		this->addRegister(it->first, *regTmp);
		this->regsToDelete.push_back(regTmp);
	}
	// Copying register files
	for (rfmap_t::const_iterator it = src.registerFiles.cbegin();
			it != src.registerFiles.cend(); ++it) {
		RegisterFile* regFileTmp = new RegisterFile(it->second);
		this->addRegisterFile(it->first, *regFileTmp);
		this->regFilesToDelete.push_back(regFileTmp);
	}
}

RegisterFile::~RegisterFile() {
	for (std::vector<Register*>::iterator it = regsToDelete.begin();
			it != regsToDelete.end(); it++) {
		delete *it;
	}
	for (std::vector<RegisterFile*>::iterator it = regFilesToDelete.begin();
			it != regFilesToDelete.end(); it++) {
		delete *it;
	}
}

std::string RegisterFile::getName() const {
	return this->name;
}

std::string RegisterFile::getDescription() const {
	return this->description;
}

std::size_t RegisterFile::getAlignment() const {
	return this->alignment;
}

std::size_t RegisterFile::getFixedSize() const {
	return this->fixedSize;
}

bool RegisterFile::isEmpty() const {
	return (registers.empty() && registerFiles.empty());
}

hvaddr_t RegisterFile::getEndAddress(const hvaddr_t &startAddr,
		const std::size_t &blockSize) const {
	return startAddr + blockSize - 1;
}

bool RegisterFile::recover(const hvaddr_t &startAddr1,
		const std::size_t &blockSize1, const hvaddr_t &startAddr2,
		const std::size_t &blockSize2) const {
	bool order = (startAddr1 <= startAddr2);
	bool ret;
	if (order) {
		ret = (startAddr2 <= this->getEndAddress(startAddr1, blockSize1));
	} else {
		ret = (startAddr1 <= this->getEndAddress(startAddr2, blockSize2));
	}
	return ret;
}

hvaddr_t RegisterFile::getLastOccupiedAddress() const {
	hvaddr_t ret = 0;
	// Checking in registers
	if (!registers.empty()) {
		rmap_t::const_iterator it = --registers.cend();
		ret = getEndAddress(it->first, it->second.getSizeInBytes());
	}
	// Checking in registerFiles
	if (!registerFiles.empty()) {
		rfmap_t::const_iterator it2 = --registerFiles.cend();
		hvaddr_t tmp(getEndAddress(it2->first, it2->second.fixedSize));
		if (ret < tmp) {
			ret = tmp;
		}
	}
	return ret;
}

bool RegisterFile::isSpaceFree(const hvaddr_t &startAddr,
		const std::size_t &blockSize) const {
	// Checking in register files map
	// We suppose a free space in an inner RegisterFile is not free for insertion
	for (rfmap_t::const_iterator it = registerFiles.cbegin();
			it != registerFiles.cend(); ++it) {
		hvaddr_t startAddrTmp = it->first;
		std::size_t sizeTmp = it->second.fixedSize;
		if (recover(startAddr, blockSize, startAddrTmp, sizeTmp)) {
			return false;
		}

	}
	// Checking in registers map
	for (rmap_t::const_iterator it = registers.begin(); it != registers.end();
			++it) {
		hvaddr_t startAddrTmp = it->first;
		std::size_t sizeTmp = it->second.getSizeInBytes();
		if (recover(startAddr, blockSize, startAddrTmp, sizeTmp)) {
			return false;
		}

	}
	// If we are here, space is free
	return true;
}

bool RegisterFile::isAligned(const hvaddr_t &startAddr,
		const std::size_t &alignSize) const {
	// Taking direct superior power of 2 of alignment
	hvaddr_t adjustedAlign = static_cast<hvaddr_t>(superiorPowerOf2(alignSize));
	bool ret = !(startAddr % adjustedAlign);
	if (alignment) { // Manually defined alignment
		ret = ret && !(startAddr % static_cast<hvaddr_t>(alignment));
	}
	return ret;
}

hvaddr_t RegisterFile::getNearestSuperiorAlignedAddress(const hvaddr_t &addr,
		const std::size_t &alignSize) const {
	hvaddr_t ret = addr;
	hvaddr_t adjustedAlign = static_cast<hvaddr_t>(superiorPowerOf2(alignSize));
	hvaddr_t remaining = addr % adjustedAlign;
	if (remaining) {
		ret += adjustedAlign - remaining;
	}
	return ret;
}

bool RegisterFile::addRegister(const hvaddr_t &insertAddr, Register &reg) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_ERR(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		exit(EXIT_FAILURE);
	}
	std::size_t alignmentTmp = alignment ? alignment : reg.getSizeInBytes();
	// Checking if the insertion address is aligned
	if (!this->isAligned(insertAddr, alignmentTmp)) {
		HV_WARN("Register insertion attempt to unaligned address")
		return false;
	}

	if (!this->isSpaceFree(insertAddr, alignmentTmp)) {
		HV_WARN("Register insertion attempt to occupied space")
		return false;
	}
	// Inserting register and returning true if succeeded
	// Inserting in registers list
	std::pair<rmap_t::iterator, bool> ret = registers.insert(
			std::pair<hvaddr_t, Register&>(insertAddr, reg));
	// Inserting in list of all registers
	if (ret.second)
		ret = allRegisters.insert(
				std::pair<hvaddr_t, Register&>(insertAddr, reg));
	return ret.second;
}

bool RegisterFile::addRegister(Register &reg) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_WARN(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		return false;
	}
	hvaddr_t insertAddrTmp(
			this->isEmpty() ?
					static_cast<hvaddr_t>(0) :
					this->getLastOccupiedAddress() + 1);
	std::size_t alignmentTmp = alignment ? alignment : reg.getSizeInBytes();
	insertAddrTmp = this->getNearestSuperiorAlignedAddress(insertAddrTmp,
			alignmentTmp);
	return this->addRegister(insertAddrTmp, reg);
}

bool RegisterFile::addRegisterCopy(const hvaddr_t &insertAddr,
		const Register &reg) {
	Register *regCp = new Register(reg);
	bool ret = this->addRegister(insertAddr, *regCp);
	if (!ret) {
		delete regCp;

	} else
		this->regsToDelete.push_back(regCp);
	return ret;
}

bool RegisterFile::addRegisterCopy(const Register &reg) {
	Register *regCp = new Register(reg);
	bool ret = this->addRegister(*regCp);
	if (!ret) {
		delete regCp;
	} else {
		this->regsToDelete.push_back(regCp);
	}
	return ret;
}

bool RegisterFile::createRegister(const hvaddr_t &address,
		const std::size_t &sizeIn, const std::string &nameIn,
		const std::string &descriptionIn, const hvrwmode_t &RWModeIn) {
	return this->createRegister(address, sizeIn, nameIn, descriptionIn,
			RWModeIn, 0u);
}

bool RegisterFile::createRegister(const hvaddr_t &address,
		const std::size_t &sizeIn, const std::string &nameIn,
		const std::string &descriptionIn, const hvrwmode_t &RWModeIn,
		const BitVector &resetIn) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_ERR(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		exit(EXIT_FAILURE);
	}
	Register *r = new Register(sizeIn, nameIn, descriptionIn, RWModeIn,
			resetIn);
	bool ret = addRegister(address, *r);
	if (!ret) {
		delete r;
	} else {
		this->regsToDelete.push_back(r);
	}
	return ret;

}

bool RegisterFile::createRegister(const std::size_t &sizeIn,
		const std::string &nameIn, const std::string &descriptionIn,
		const hvrwmode_t &RWModeIn, const BitVector &resetIn) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_ERR(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		exit(EXIT_FAILURE);
	}
	hvaddr_t address(this->isEmpty() ? 0 : this->getLastOccupiedAddress() + 1);
	return createRegister(address, sizeIn, nameIn, descriptionIn, RWModeIn,
			resetIn);
}

bool RegisterFile::createRegisterBlock(const hvaddr_t &address,
		const std::size_t &nBlock, const std::size_t &sizeIn,
		const std::string &nameIn, const std::string &descriptionIn,
		const hvrwmode_t &RWModeIn, const BitVector &resetIn) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_ERR(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		exit(EXIT_FAILURE);
	}
	std::size_t alignmentTmp = alignment ? alignment : sizeIn;
	// Checking if the insertion address is aligned
	if (!this->isAligned(address, alignmentTmp)) {
		HV_ERR("Register creation attempt to unaligned address")
		return false;
	}
	hvaddr_t addrTmp = address;
	for (std::size_t i = 0; i < nBlock; i++) {
		addrTmp = this->getNearestSuperiorAlignedAddress(addrTmp, alignmentTmp);
		bool ret = createRegister(addrTmp, sizeIn,
				nameIn + "_" + std::to_string(i), descriptionIn, RWModeIn,
				resetIn);
		if (!ret) {
			return false;
		}
		addrTmp += (sizeIn % 8) ? sizeIn / 8 + 1 : sizeIn / 8;
	}
	return true;
}

bool RegisterFile::createRegisterBlock(const hvaddr_t &address,
		const std::size_t &nBlock, const std::size_t &sizeIn,
		const std::string &nameIn, const std::string &descriptionIn,
		const hvrwmode_t &RWModeIn) {
	return this->createRegisterBlock(address, nBlock, sizeIn, nameIn,
			descriptionIn, RWModeIn, 0u);
}

bool RegisterFile::createRegisterBlock(const std::size_t &nBlock,
		const std::size_t &sizeIn, const std::string &nameIn,
		const std::string &descriptionIn, const hvrwmode_t &RWModeIn,
		const BitVector &resetIn) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_WARN(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		return false;
	}
	hvaddr_t address(this->isEmpty() ? 0 : this->getLastOccupiedAddress() + 1);
	return createRegisterBlock(address, nBlock, sizeIn, nameIn, descriptionIn,
			RWModeIn, resetIn);
}

bool RegisterFile::addRegisterFile(const hvaddr_t &insertAddr,
		RegisterFile &regFile) {
	return this->addRegisterFile(insertAddr, regFile, std::size_t(0));
}

bool RegisterFile::addRegisterFile(const hvaddr_t &insertAddr,
		RegisterFile &regFile, const std::size_t &regFileSize) {
	// Checking if current RegisterFile is not locked
	if (this->fixedSize != std::size_t(0)) {
		HV_WARN(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		return false;
	}
	// Checking if alignments are the same
	if (this->alignment != regFile.getAlignment()) {
		HV_WARN(
				"Alignment in inserted register file must be the same than its container's alignment")
		return false;
	}
	// Setting up size
	std::size_t sizeTmp;
	if (regFile.fixedSize) {
		/*
		 * This means that the register file to insert
		 * has already been inserted somewhere else.
		 * Therefore its width is already fixed
		 */
		sizeTmp = regFile.fixedSize;
	} else {
		/*
		 * Else its size is fixed forever
		 */
		sizeTmp =
				regFile.isEmpty() ?
						static_cast<hvaddr_t>(0) :
						regFile.getLastOccupiedAddress() + 1;
		if (regFileSize) {
			if (regFileSize < sizeTmp) {
				HV_WARN(
						"Specified register file size is inferior to the space it actually occupies")
				return false;
			}
			sizeTmp = regFileSize;
		}
		regFile.fixedSize = sizeTmp;
	}

	// Checking for alignment
	if (this->alignment == std::size_t(0)) {
		if (!isAligned(insertAddr, sizeTmp)) {
			HV_WARN("Register file insertion attempt to unaligned address")
			return false;
		}
	} else {
		if (!isAligned(insertAddr, alignment)) {
			HV_WARN("Register file insertion attempt to unaligned address")
			return false;
		}
	}

	// Checking if space is free
	if (!isSpaceFree(insertAddr, sizeTmp)) {
		HV_WARN("Space is not free for register file insertion")
		return false;
	}

	// Inserting
	std::pair<rfmap_t::iterator, bool> ret = registerFiles.insert(
			std::pair<hvaddr_t, RegisterFile&>(insertAddr, regFile));
	if (ret.second) {
		/*
		 * Adding all registers in the hierarchy of added
		 * register file to allRegisters map
		 */
		std::pair<rmap_t::iterator, bool> regTmp;
		for (rmap_t::iterator it = regFile.allRegisters.begin();
				it != regFile.allRegisters.end(); ++it) {
			regTmp = allRegisters.insert(
					std::pair<hvaddr_t, Register&>(it->first + insertAddr,
							it->second));
			if (!regTmp.second) {
				HV_WARN("Problem reading hierarchy of inserted register file")
				return false;
			}
		}
	}
	return ret.second;
}

bool RegisterFile::addRegisterFile(RegisterFile &regFile) {
	return this->addRegisterFile(regFile, std::size_t(0));
}

bool RegisterFile::addRegisterFile(RegisterFile &regFile,
		const std::size_t &regFileSize) {
	// Checking if current RegisterFile is not locked
	if (this->getFixedSize() != std::size_t(0)) {
		HV_WARN(
				"Impossible to add a register (RegisterFile locked after first insertion)")
		return false;
	}
	hvaddr_t insertAddrTmp(
			this->isEmpty() ?
					static_cast<hvaddr_t>(0) :
					this->getLastOccupiedAddress() + 1);
	// Post-checking alignment to find address where to insert
	std::size_t alignmentTmp =
			alignment ? alignment :
			regFile.fixedSize ? fixedSize :
			regFileSize ?
					regFileSize :
					static_cast<std::size_t>(
							regFile.isEmpty() ?
									static_cast<hvaddr_t>(0) :
									regFile.getLastOccupiedAddress() + 1);
	insertAddrTmp = this->getNearestSuperiorAlignedAddress(insertAddrTmp,
			alignmentTmp);
	return this->addRegisterFile(insertAddrTmp, regFile, regFileSize);
}

hvaddr_t RegisterFile::getRegisterAddress(const std::string &name) const {
	hvaddr_t ret;
	bool found = false;
	for (rmap_t::const_iterator it = this->allRegisters.cbegin();
			(!found) && (it != this->allRegisters.cend()); ++it) {
		if (!std::strcmp(name.c_str(), it->second.getName().c_str())) {
			found = true;
			ret = it->first;
		}
	}
	if (!found) {
		HV_ERR("No register named " << name << " in register file")
		exit(EXIT_FAILURE);
	}

	return ret;
}

hvaddr_t RegisterFile::getRegisterFileAddress(const std::string &name) const {
	hvaddr_t ret;
	bool found = false;
	for (rfmap_t::const_iterator it = this->registerFiles.cbegin();
			(!found) && (it != this->registerFiles.cend()); ++it) {
		if (!std::strcmp(name.c_str(), it->second.getName().c_str())) {
			found = true;
			ret = it->first;
		}
	}
	if (!found) {
		HV_ERR("No register file named " << name << " in register file")
		exit(EXIT_FAILURE);
	}
	return ret;
}

Register& RegisterFile::getRegister(const hvaddr_t &address) const {
	rmap_t::const_iterator regIt = this->allRegisters.find(address);
	if (regIt == this->allRegisters.end()) {
		// Definitely not found.
		HV_ERR("No register @" << std::hex << std::uppercase << "0x" << address);
		exit(EXIT_FAILURE);
	}
	Register& ret = regIt->second;
	return ret;
}

Register& RegisterFile::getRegister(const std::string &name) const {
	hvaddr_t addr = this->getRegisterAddress(name);
	return this->getRegister(addr);
}

RegisterFile& RegisterFile::getRegisterFile(const hvaddr_t &address) const {
	rfmap_t::const_iterator tmp = registerFiles.find(address);
	HV_ASSERT(tmp != registerFiles.end(),
			"No register file @" << std::hex << std::uppercase << "0x" << address)
	RegisterFile &ret = tmp->second;
	return ret;
}

RegisterFile& RegisterFile::getRegisterFile(const std::string &name) const {
	hvaddr_t addrTmp = this->getRegisterFileAddress(name);
	return this->getRegisterFile(addrTmp);
}

Register& RegisterFile::operator()(const hvaddr_t &address) const {
	return this->getRegister(address);
}

Register& RegisterFile::operator()(const std::string &name) const {
	return this->getRegister(name);
}

RegisterFile& RegisterFile::operator [](const hvaddr_t &address) const {
	return this->getRegisterFile(address);
}

RegisterFile& RegisterFile::operator [](const std::string &name) const {
	return this->getRegisterFile(name);
}

bool RegisterFile::read(const hvaddr_t &address, hvuint8_t* readBuff,
		const std::size_t &readSize) {
	Register &retTmp = this->getRegister(address);
	return retTmp.read(readBuff, readSize);
}

bool RegisterFile::write(const hvaddr_t &address, const hvuint8_t* writeBuff,
		const std::size_t &writeSize) {
	Register &retTmp = this->getRegister(address);
	return retTmp.write(writeBuff, writeSize);
}

std::string RegisterFile::getInfo() const {
	std::map<hvaddr_t, hvaddr_t> endAddressMap;
	std::map<hvaddr_t, std::string> nameMap;
	std::map<hvaddr_t, std::string> descriptionMap;
	std::map<hvaddr_t, std::size_t> sizeMap;
	std::map<hvaddr_t, bool> typeMap;
	std::stringstream ret, tmp;
	for (rmap_t::const_iterator it = registers.cbegin(); it != registers.cend();
			it++) {
		endAddressMap[it->first] = this->getEndAddress(it->first,
				it->second.getSizeInBytes());
		nameMap[it->first] = it->second.getName();
		descriptionMap[it->first] = it->second.getDescription();
		sizeMap[it->first] = it->second.getSizeInBytes();
		typeMap[it->first] = true;
	}
	for (rfmap_t::const_iterator it = registerFiles.cbegin();
			it != registerFiles.cend(); it++) {
		endAddressMap[it->first] = it->first + it->second.fixedSize - 1;
		nameMap[it->first] = it->second.getName();
		descriptionMap[it->first] = it->second.getDescription();
		sizeMap[it->first] = it->second.getFixedSize();
		typeMap[it->first] = false;
	}
	TextTable t('-', '|', '+');
	t.add("Rel. address");
	t.add("Type");
	t.add("Size");
	t.add("Name");
	t.add("Description");
	t.endOfRow();
	for (std::map<hvaddr_t, hvaddr_t>::const_iterator it =
			endAddressMap.cbegin(); it != endAddressMap.cend(); it++) {
		tmp << std::hex << std::uppercase << "0x" << it->first << " -> 0x"
				<< it->second;
		t.add(tmp.str());
		tmp.str("");
		tmp.clear();
		t.add(
				typeMap[it->first] ?
						std::string("Reg.") : std::string("Reg. File"));
		t.add(
				std::to_string(sizeMap[it->first]) + " byte"
						+ (sizeMap[it->first] == std::size_t(1) ? "" : "s"));
		t.add(nameMap[it->first]);
		t.add(descriptionMap[it->first]);
		t.endOfRow();
	}
	ret << "Register file:" << std::endl;
	ret << "\tName: " << this->name << std::endl;
	ret << "\tDescription: " << this->description << std::endl;
	ret << t;
	return ret.str();
}

std::string RegisterFile::getInfo(const std::string &nameIn) const {
	std::string ret = getRegister(nameIn).getInfo();
	return ret;
}

std::string RegisterFile::getInfo(const hvaddr_t &addrIn) const {
	std::string ret = getRegister(addrIn).getInfo();
	return ret;
}

std::string RegisterFile::getRecursiveInfo(unsigned int level) const {
	std::stringstream ret;
	ret << "############## Register files details - level " << level
			<< " ##############" << std::endl;
	ret << this->getInfo();
	if (this->registerFiles.size() > 0) {
		for (rfmap_t::const_iterator it = this->registerFiles.cbegin();
				it != this->registerFiles.cend(); it++) {
			ret << it->second.getRecursiveInfo(level + 1);
		}
	}
	if (!level) {
		ret << "##############################################################"
				<< std::endl;
	}
	return ret.str();
}

std::string RegisterFile::getFlattenedInfo() const {
	std::map<hvaddr_t, hvaddr_t> endAddressMap;
	std::map<hvaddr_t, std::string> nameMap;
	std::map<hvaddr_t, std::string> descriptionMap;
	std::map<hvaddr_t, std::size_t> sizeMap;
	std::stringstream ret, tmp;
	for (rmap_t::const_iterator it = allRegisters.cbegin();
			it != allRegisters.cend(); it++) {
		endAddressMap[it->first] = this->getEndAddress(it->first,
				it->second.getSizeInBytes());
		nameMap[it->first] = it->second.getName();
		descriptionMap[it->first] = it->second.getDescription();
		sizeMap[it->first] = it->second.getSizeInBytes();
	}

	TextTable t('-', '|', '+');
	t.add("Rel. address");
	t.add("Type");
	t.add("Size");
	t.add("Name");
	t.add("Description");
	t.endOfRow();
	for (std::map<hvaddr_t, hvaddr_t>::const_iterator it =
			endAddressMap.cbegin(); it != endAddressMap.cend(); it++) {
		tmp << std::hex << std::uppercase << "0x" << it->first << " -> 0x"
				<< it->second;
		t.add(tmp.str());
		tmp.str("");
		tmp.clear();
		t.add(std::string("Reg."));
		t.add(
				std::to_string(sizeMap[it->first]) + " byte"
						+ (sizeMap[it->first] == std::size_t(1) ? "" : "s"));
		t.add(nameMap[it->first]);
		t.add(descriptionMap[it->first]);
		t.endOfRow();
	}
	ret << "Register file:" << std::endl;
	ret << "\tName: " << this->name << std::endl;
	ret << "\tDescription: " << this->description << std::endl;
	ret << t;
	return ret.str();
}
} // namespace reg
} // namespace hv
