/**
 * @file register.h
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date Jun, 2017
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register class
 */

#include "register/register.h"

using namespace ::hv::common;

namespace hv {
namespace reg {

Register::Register(const std::size_t sizeIn, const std::string &nameIn,
		const std::string &descriptionIn, const hvrwmode_t &modeIn,
		const BitVector &resetIn) :
		name(nameIn), description(descriptionIn), mode(modeIn), data(sizeIn,
				resetIn), resetVal(BitVector(sizeIn, resetIn)), readMask(sizeIn,
				~BitVector(sizeIn, 0u)), writeMask(sizeIn,
				~BitVector(sizeIn, 0u)), readLock(false), writeLock(false), cbIDCpt(
				0u), regCCI(*this) {
	if (mode == RO) {
		writeMask = 0u;
	} else if (mode == WO) {
		readMask = 0u;
	}
}

Register::Register(const Register& src) :
		name(src.name), description(src.description), mode(src.mode), data(
				src.data), resetVal(src.resetVal), readMask(src.readMask), writeMask(
				src.writeMask), fields(src.fields), readLock(false), writeLock(
				false), cbIDCpt(0u), regCCI(*this) {
	// Warning - callbacks are not copied when copying registers
}

Register::~Register() {
}

std::size_t Register::getSize() const {
	return data.getSize();
}

std::size_t Register::getSizeInBytes() const {
	std::size_t sizeTmp(this->getSize());
	return (sizeTmp % 8) ? sizeTmp / 8 + 1 : sizeTmp / 8;
}

std::string Register::getName() const {
	return name;
}

std::string Register::getDescription() const {
	return description;
}

hvrwmode_t Register::getRWMode() const {
	return mode;
}

BitVector Register::getReadMask() const {
	return readMask;
}

BitVector Register::getWriteMask() const {
	return writeMask;
}

BitVector Register::getValue(const bool &applyReadMask) const {
	BitVector ret(this->getSize(), data);
	if (applyReadMask)
		ret &= readMask;
	return ret;
}

void Register::setResetValue(const BitVector &resetIn) {
	resetVal = resetIn;
}

void Register::setReadMask(const BitVector &readMaskVal) {
	readMask = readMaskVal;
}

void Register::setWriteMask(const BitVector &writeMaskVal) {
	writeMask = writeMaskVal;
}

void Register::setValue(const BitVector &src, const bool &applyWriteMask) {
	if (applyWriteMask) {
		// Clearing bits to be overrided
		data &= ~writeMask;
		// Writing value
		data |= src & writeMask;
	} else {
		data = src;
	}
}

void Register::reset() {
	data = resetVal;
}

bool Register::read(hvuint8_t* readBuff, const std::size_t &readSize) {
	// We have to do this to avoid creating a cci event when there is no CCI callback registered
	bool hasCCICallbacks = regCCI.hasCallbacks();
	bool preReadOK(true);

	// Pre-read callbacks execution
	if (!readLock) {
		readLock = true;
		BitVector evValue(this->data);
		RegisterReadEvent ev(evValue, *this);
		preReadOK = this->runPreReadCallbacks(ev);
		if (preReadOK && hasCCICallbacks) {
			::hv::hvcci::RegisterReadEvent<> cciEvent(::cci::cci_value(evValue),
					regCCI);
			preReadOK = regCCI.runPreReadCallbacks(cciEvent);
		}
		readLock = false;
	}
	if (preReadOK) {
		// Reading
		// Creating tmp variable for mask application
		BitVector bvTmp(this->data);
		// Applying mask
		bvTmp &= readMask;
		void *tmp = std::memcpy(readBuff, bvTmp.getDataAddress(), readSize);
		if (tmp == nullptr) {
			HV_ERR("Reading register data failed");
			exit(EXIT_FAILURE);
		}

		// Post-read callbacks execution
		if (!readLock) {
			readLock = true;
			BitVector evValue = this->data;
			RegisterReadEvent ev(evValue, *this);
			this->runPostReadCallbacks(ev);
			if (hasCCICallbacks) {
				::hv::hvcci::RegisterReadEvent<> cciEvent(
						::cci::cci_value(evValue), regCCI);
				this->runPostReadCallbacks(ev);
			}
			readLock = false;
		}
	}
	return preReadOK;
}

bool Register::write(const hvuint8_t* writeBuff, const std::size_t &writeSize) {
	// Saving old value
	BitVector oldVal(this->data);
	// Creating new value
	BitVector newVal(this->data);
	BitVector bvTmp(this->getSize(), 0u);
	bool preWriteOK(true);

	// Pre-writing to feed event
	void *tmp = std::memcpy(bvTmp.getDataAddress(), writeBuff, writeSize);
	if (tmp == nullptr) {
		HV_ERR("Writing data to register failed");
		exit(EXIT_FAILURE);
	}
	// Applying write mask to bvTmp
	bvTmp &= writeMask;
	// Clearing newVal bits to be overrided
	newVal &= ~writeMask;
	// Writing value
	newVal |= bvTmp;

	bool hasCCICallbacks = regCCI.hasCallbacks();
	// Pre-write callbacks execution
	if (!writeLock) {
		writeLock = true;
		RegisterWriteEvent ev(oldVal, newVal, *this);
		preWriteOK = this->runPreWriteCallbacks(ev);
		if (preWriteOK && hasCCICallbacks) {
			::hv::hvcci::RegisterWriteEvent<> cciEvent(::cci::cci_value(oldVal),
					::cci::cci_value(newVal), regCCI);
			preWriteOK = regCCI.runPreWriteCallbacks(cciEvent);
		}
		writeLock = false;

	}
	if (preWriteOK) {
		// Writing data
		this->data = newVal;
		// Post-write callbacks execution
		// Note: write event can't have changed so we don't recreate it
		if (!writeLock) {
			writeLock = true;
			RegisterWriteEvent ev(oldVal, newVal, *this);
			this->runPostWriteCallbacks(ev);
			if (hasCCICallbacks) {
				::hv::hvcci::RegisterWriteEvent<> cciEvent(
						::cci::cci_value(oldVal), ::cci::cci_value(newVal),
						regCCI);
				regCCI.runPostWriteCallbacks(cciEvent);
			}
			writeLock = false;
		}
	}
	return preWriteOK;
}

bool Register::read(BitVector& dest) {
	HV_ASSERT(dest.getSize() == this->getSize(),
			"Destination BitVector must be the same size as the Register read from")
	hvuint8_t* readBuff = (hvuint8_t*) malloc(
			sizeof(hvuint8_t) * this->getSizeInBytes());
	if (readBuff == nullptr) {
		HV_ERR("Error while allocating resource")
		exit(EXIT_FAILURE);
	}
	if (this->read(readBuff, this->getSizeInBytes())) {
		dest = readBuff[0];
		for (hvuint32_t i = 1u; i < this->getSizeInBytes(); i++) {
			BitVector tmp(this->getSize(), readBuff[i]);
			tmp <<= (8u * i);
			dest |= tmp;
		}
		return true;
	}
	return false;
}

bool Register::write(const BitVector& src) {
	HV_ASSERT(src.getSize() == this->getSize(),
			"Destination BitVector must be the same size as the Register read from")
	hvuint8_t* writeBuff = (hvuint8_t*) malloc(
			sizeof(hvuint8_t) * this->getSizeInBytes());
	if (writeBuff == nullptr) {
		HV_ERR("Error while allocating resource")
		exit(EXIT_FAILURE);
	}
	for (hvuint32_t i = 0; i < this->getSizeInBytes(); i++) {
		writeBuff[i] = src >> (8u * i);
	}
	if (this->write(writeBuff, this->getSizeInBytes())) {
		return true;
	}
	return false;
}

#define HV_REG_CAST_TO(T) Register::operator T() const { return T(data); }
HV_REG_CAST_TO(bool)
HV_REG_CAST_TO(hvuint8_t)
HV_REG_CAST_TO(hvuint16_t)
HV_REG_CAST_TO(hvuint32_t)
HV_REG_CAST_TO(hvuint64_t)
HV_REG_CAST_TO(hvint8_t)
HV_REG_CAST_TO(hvint16_t)
HV_REG_CAST_TO(hvint32_t)
HV_REG_CAST_TO(hvint64_t)
HV_REG_CAST_TO(std::string)

#define HV_REG_OPERATOR_EQUAL(T) Register& Register::operator =(const T &src) {data = src; return *this;}
HV_REG_OPERATOR_EQUAL(bool)
HV_REG_OPERATOR_EQUAL(hvuint8_t)
HV_REG_OPERATOR_EQUAL(hvuint16_t)
HV_REG_OPERATOR_EQUAL(hvuint32_t)
HV_REG_OPERATOR_EQUAL(hvuint64_t)
HV_REG_OPERATOR_EQUAL(hvint8_t)
HV_REG_OPERATOR_EQUAL(hvint16_t)
HV_REG_OPERATOR_EQUAL(hvint32_t)
HV_REG_OPERATOR_EQUAL(hvint64_t)
HV_REG_OPERATOR_EQUAL(BitVector)
HV_REG_OPERATOR_EQUAL(std::string)

Register& Register::operator =(const Register &src) {
	data = src.data;
	return *this;
}

BitVector Register::operator <<(const hvuint32_t &nShift) const {
	return data << nShift;
}

BitVector Register::operator <<(const hvint32_t &nShift) const {
	return data << nShift;
}

BitVector Register::operator >>(const hvuint32_t &nShift) const {
	return data >> nShift;
}

BitVector Register::operator >>(const hvint32_t &nShift) const {
	return data >> nShift;
}

Register& Register::operator <<=(const hvuint32_t &nShift) {
	data <<= nShift;
	return *this;
}

Register& Register::operator <<=(const hvint32_t &nShift) {
	data <<= nShift;
	return *this;
}

Register& Register::operator >>=(const hvuint32_t &nShift) {
	data >>= nShift;
	return *this;
}

Register& Register::operator >>=(const hvint32_t &nShift) {
	data >>= nShift;
	return *this;
}

bool Register::operator !() const {
	return !data;
}

bool Register::operator ==(const Register &op2) const {
	return data == op2.data;
}

bool Register::operator !=(const Register &op2) const {
	return data != op2.data;
}

bool Register::operator &&(const Register &op2) const {
	return data && op2.data;
}

bool Register::operator ||(const Register &op2) const {
	return data || op2.data;
}

BitVector Register::operator ~() const {
	return ~data;
}

BitVector Register::operator &(const Register &op2) const {
	return data & op2.data;
}

BitVector Register::operator |(const Register &op2) const {
	return data | op2.data;
}

BitVector Register::operator ^(const Register &op2) const {
	return data ^ op2.data;
}

Register& Register::operator &=(const Register &op2) {
	data &= op2.data;
	return *this;
}

Register& Register::operator |=(const Register &op2) {
	data |= op2.data;
	return *this;
}

Register& Register::operator ^=(const Register &op2) {
	data ^= op2.data;
	return *this;
}

BitVector Register::operator +(const Register &op2) const {
	return data + op2.data;
}

BitVector Register::operator ()(const std::size_t &ind1,
		const std::size_t &ind2) {
	return data(ind1, ind2);
}

BitVector Register::operator ()(const std::size_t &ind1,
		const std::size_t &ind2) const {
	return data(ind1, ind2);
}

BitVector Register::operator [](const std::size_t &ind) {
	return data[ind];
}

BitVector Register::operator [](const std::size_t &ind) const {
	return data[ind];
}

void Register::createField(const std::string &fieldName,
		const std::size_t &ind1, const std::size_t &ind2,
		const std::string &fieldDescription, const hvrwmode_t &fieldRWMode) {
	std::size_t MSB(ind1);
	std::size_t LSB(ind2);
	hvrwmode_t modeTmp = fieldRWMode;
	if (modeTmp == NA) {
		if (mode == NA) {
			HV_WARN(
					"Nor register neither field has read/write mode defined." << std::endl << "Field mode will be considered as RW")
			modeTmp = RW;
		}
	} else if (modeTmp == RW) {
		if (mode == RO) {
			HV_ERR(
					"A RO-mode register can't have RW field." << std::endl << "You can fix this by setting register mode to RW or NA")
			exit(EXIT_FAILURE);
		} else if (mode == WO) {
			HV_ERR(
					"A WO-mode register can't have RW field." << std::endl << "You can fix this by setting register mode to RW or NA")
			exit(EXIT_FAILURE);
		}
	} else if (modeTmp == RO) {
		if (mode == RW) {
			HV_ERR(
					"A RW-mode register can't have RO field." << std::endl << "You can fix this by setting register mode to RO or NA")
			exit(EXIT_FAILURE);
		} else if (mode == WO) {
			HV_ERR(
					"A WO-mode register can't have RO field." << std::endl << "You can fix this by setting register mode to RO or NA")
			exit(EXIT_FAILURE);
		}
	} else { // modeTmp == WO
		if (mode == RW) {
			HV_ERR(
					"A RW-mode register can't have WO field." << std::endl << "You can fix this by setting register mode to WO or NA")
			exit(EXIT_FAILURE);
		} else if (mode == RO) {
			HV_ERR(
					"A RO-mode register can't have WO field." << std::endl << "You can fix this by setting register mode to WO or NA")
			exit(EXIT_FAILURE);
		}
	}
	if (ind1 < ind2) {
		std::size_t tmp(MSB);
		MSB = LSB;
		LSB = tmp;
	}
	fields.add(fieldName, LSB, MSB, fieldDescription, modeTmp);
	this->updateMasks();
}

void Register::createField(const std::string &fieldName, const std::size_t &ind,
		const std::string &fieldDescription, const hvrwmode_t &fieldRWMode) {
	this->createField(fieldName, ind, ind, fieldDescription, fieldRWMode);
}

void Register::createField(const std::string &fieldName,
		const std::size_t &ind1, const std::size_t &ind2,
		const hvrwmode_t &fieldRWMode) {
	this->createField(fieldName, ind1, ind2, std::string(""), fieldRWMode);
}

void Register::createField(const std::string &fieldName, const std::size_t &ind,
		const hvrwmode_t &fieldRWMode) {
	this->createField(fieldName, ind, ind, std::string(""), fieldRWMode);
}

void Register::createField(const std::string &fieldName,
		const std::size_t &ind1, const std::size_t &ind2) {
	this->createField(fieldName, ind1, ind2, std::string(""), hvrwmode_t::NA);

}

BitVector Register::operator ()(const std::string &fieldName) {
	std::size_t indLow, indHigh;
	hvrwmode_t RWmodeTmp;
	if (!fields.get(fieldName, &indLow, &indHigh, &RWmodeTmp)) {
		HV_ERR("Field does not exist")
		exit(EXIT_FAILURE);
	}
	return data(indLow, indHigh);
}

BitVector Register::operator ()(const std::string &fieldName) const {
	std::size_t indLow, indHigh;
	hvrwmode_t RWmodeTmp;
	if (!fields.get(fieldName, &indLow, &indHigh, &RWmodeTmp)) {
		HV_ERR("Field does not exist")
		exit(EXIT_FAILURE);
	}
	return data(indLow, indHigh);
}

std::pair<std::size_t, std::size_t> Register::getFieldIndexes(
		const std::string &fieldName) const {
	std::size_t indLow, indHigh;
	hvrwmode_t RWmodeTmp;
	if (!fields.get(fieldName, &indLow, &indHigh, &RWmodeTmp)) {
		HV_ERR("Field does not exist")
		exit(EXIT_FAILURE);
	}
	return std::pair<std::size_t, std::size_t>(indLow, indHigh);
}

std::size_t Register::howManyPreReadCallbacks() const {
	return preReadCbVect.size();
}

std::size_t Register::howManyPostReadCallbacks() const {
	return postReadCbVect.size();
}

std::size_t Register::howManyPreWriteCallbacks() const {
	return preWriteCbVect.size();
}

std::size_t Register::howManyPostWriteCallbacks() const {
	return postWriteCbVect.size();
}

std::size_t Register::howManyCallbacks() const {
	return howManyPreReadCallbacks() + howManyPostReadCallbacks()
			+ howManyPreWriteCallbacks() + howManyPostWriteCallbacks();
}

hvcbID_t Register::registerPreReadCallback(const PreReadCallback &cb) {
	PreReadCallback cbTmp(cb);
	hvcbID_t idTmp = this->getUniqueID();
	cbTmp.setId(idTmp);
	preReadCbVect.push_back(cbTmp);
	return idTmp;
}

hvcbID_t Register::registerPostReadCallback(const PostReadCallback &cb) {
	PostReadCallback cbTmp(cb);
	hvcbID_t idTmp = this->getUniqueID();
	cbTmp.setId(idTmp);
	postReadCbVect.push_back(cbTmp);
	return idTmp;
}

hvcbID_t Register::registerPreWriteCallback(const PreWriteCallback &cb) {
	PreWriteCallback cbTmp(cb);
	hvcbID_t idTmp = this->getUniqueID();
	cbTmp.setId(idTmp);
	preWriteCbVect.push_back(cbTmp);
	return idTmp;
}

hvcbID_t Register::registerPostWriteCallback(const PostWriteCallback &cb) {
	PostWriteCallback cbTmp(cb);
	hvcbID_t idTmp = this->getUniqueID();
	cbTmp.setId(idTmp);
	postWriteCbVect.push_back(cbTmp);
	return idTmp;
}

bool Register::unregisterPreReadCallback(const hvcbID_t &id) {
	PreReadCallbackVector::iterator it = getPreReadCallbackIt(id);
	if (it == preReadCbVect.cend()) {
		return false;
	}
	preReadCbVect.erase(it);
	return true;
}

bool Register::unregisterPostReadCallback(const hvcbID_t &id) {
	PostReadCallbackVector::iterator it = getPostReadCallbackIt(id);
	if (it == postReadCbVect.cend()) {
		return false;
	}
	postReadCbVect.erase(it);
	return true;

}

bool Register::unregisterPreWriteCallback(const hvcbID_t &id) {
	PreWriteCallbackVector::iterator it = getPreWriteCallbackIt(id);
	if (it == preWriteCbVect.cend()) {
		return false;
	}
	preWriteCbVect.erase(it);
	return true;
}

bool Register::unregisterPostWriteCallback(const hvcbID_t &id) {
	PostWriteCallbackVector::iterator it = getPostWriteCallbackIt(id);
	if (it == postWriteCbVect.cend()) {
		return false;
	}
	postWriteCbVect.erase(it);
	return true;
}

bool Register::unregisterAllCallbacks() {
	preReadCbVect.clear();
	postReadCbVect.clear();
	preWriteCbVect.clear();
	postWriteCbVect.clear();
	return true;
}

bool Register::runPreReadCallbacks(const RegisterReadEvent& ev) {
	for (auto it = this->preReadCbVect.begin(); it != this->preReadCbVect.end();
			++it) {
		if (!(*it)(ev))
			return false;
	}
	return true;
}

void Register::runPostReadCallbacks(const RegisterReadEvent& ev) {
	for (auto it = postReadCbVect.begin(); it != postReadCbVect.end(); ++it) {
		(*it)(ev);
	}
}

bool Register::runPreWriteCallbacks(const RegisterWriteEvent& ev) {
	for (auto it = this->preWriteCbVect.begin();
			it != this->preWriteCbVect.end(); ++it) {
		if (!(*it)(ev))
			return false;
	}
	return true;
}

void Register::runPostWriteCallbacks(const RegisterWriteEvent& ev) {
	for (auto it = postWriteCbVect.begin(); it != postWriteCbVect.end(); ++it) {
		(*it)(ev);
	}
}

Register::PreReadCallbackVector::iterator Register::getPreReadCallbackIt(
		const hvcbID_t &id) {
	for (PreReadCallbackVector::iterator it = preReadCbVect.begin();
			it != preReadCbVect.end(); ++it) {
		if (it->getId() == id)
			return it;
	}
	return preReadCbVect.end();
}

Register::PostReadCallbackVector::iterator Register::getPostReadCallbackIt(
		const hvcbID_t &id) {
	for (PostReadCallbackVector::iterator it = postReadCbVect.begin();
			it != postReadCbVect.end(); ++it) {
		if (it->getId() == id)
			return it;
	}
	return postReadCbVect.end();
}

Register::PreWriteCallbackVector::iterator Register::getPreWriteCallbackIt(
		const hvcbID_t &id) {
	for (PreWriteCallbackVector::iterator it = preWriteCbVect.begin();
			it != preWriteCbVect.end(); ++it) {
		if (it->getId() == id)
			return it;
	}
	return preWriteCbVect.end();
}

Register::PostWriteCallbackVector::iterator Register::getPostWriteCallbackIt(
		const hvcbID_t &id) {
	for (PostWriteCallbackVector::iterator it = postWriteCbVect.begin();
			it != postWriteCbVect.end(); ++it) {
		if (it->getId() == id)
			return it;
	}
	return postWriteCbVect.end();
}

std::string Register::getInfo(hvuint32_t level) const {
	std::string delimiter(50, '#');
	delimiter += "\n";
	std::string ret(delimiter);
	if (level == 0u) {
		ret += this->getRegTable();
	} else if (level >= 1u) {
		std::stringstream tmp;
		bool readModeActive(true);
		bool writeModeActive(true);
		tmp << "Register name:" << std::endl << "\t" << this->name << std::endl;
		tmp << "Register description:" << std::endl << "\t";
		if (!this->description.compare(std::string(""))) {
			tmp << "<no available description>" << std::endl;
		} else {
			tmp << this->description << std::endl;
		}
		tmp << "Read/Write mode:" << std::endl << "\t";
		switch (this->mode) {
		case RW:
			tmp << "Read-Write";
			break;
		case RO:
			tmp << "Read-Only";
			writeModeActive = false;
			break;
		case WO:
			tmp << "Write-Only";
			readModeActive = false;
			break;
		default:
			tmp << "Field-level-defined";
			break;
		}
		tmp << std::endl;
		tmp << "Reset value:" << std::endl << "\t"
				<< binStrToHexaStr(resetVal.toString()) << std::endl;
		if (readModeActive) {
			tmp << "Read mask value:" << std::endl << "\t"
					<< binStrToHexaStr(readMask.toString()) << std::endl;
		}
		if (writeModeActive) {
			tmp << "Write mask value:" << std::endl << "\t"
					<< binStrToHexaStr(writeMask.toString()) << std::endl;
		}
		tmp << std::endl;

		tmp << "#### Register general view ####" << std::endl;
		tmp << this->getRegTable() << std::endl;

		tmp << "#### Register value ####" << std::endl;
		tmp << "\tBin: " << data.toString() << std::endl;
		tmp << "\tHex: " << binStrToHexaStr(data.toString()) << std::endl
				<< std::endl;

		tmp << "#### Fields details ####" << std::endl;
		if (this->fields.cbegin() == this->fields.cend()) {
			tmp << "< No fields defined >" << std::endl;
		} else {
			std::vector<std::pair<std::string, Field>> sortedFields =
					fields.getFieldsSortedByIndex(true);
			for (std::vector<std::pair<std::string, Field>>::const_iterator it =
					sortedFields.cbegin(); it != sortedFields.cend(); ++it) {
				tmp << "Field name:\n\t" << it->first << std::endl;
				tmp << "Field description:\n\t";
				if (!it->second.getDescription().compare(std::string(""))) {
					tmp << "<no available description>" << std::endl;
				} else {
					tmp << it->second.getDescription() << std::endl;
				}
				tmp << "Field position:\n\t";
				if (it->second.getIndLow() == it->second.getIndHigh()) {
					tmp << it->second.getIndLow() << std::endl;
				} else {
#ifdef HV_BV_STR_MSB_FIRST
					tmp << it->second.getIndHigh() << " <- "
							<< it->second.getIndLow() << std::endl;
#else
					tmp << it->second.getIndLow() << " -> " << it->second.getIndHigh()
					<< std::endl;
#endif
				}
				tmp << "-----------------------" << std::endl;
			}
		}
		ret += tmp.str();
	}
	ret += delimiter;
	return ret;
}

std::string Register::getRegTable() const {
	std::vector<std::pair<std::string, Field>> nonRecoveringFields =
			fields.getFieldsSortedByIndex(false);
	std::stringstream ret;
	TextTable t('-', '|', '+');
	std::size_t regSize = this->getSize();
	std::size_t prevIndHigh(0u);
	if (!nonRecoveringFields.empty()) {
		std::vector<std::pair<std::string, Field>>::iterator it =
				nonRecoveringFields.begin();
		// Checking if fields start with an empty space (LSB side)
		if (it->second.getIndLow() != 0u) {
			it = nonRecoveringFields.insert(it,
					std::pair<std::string, Field>("",
							Field(0u, it->second.getIndLow() - 1u, "", NA)));
			++it;
		}
		prevIndHigh = it->second.getIndHigh();
		++it;
		// Filling empty spaces
		while (it != nonRecoveringFields.end()) {
			if (it->second.getIndLow() > prevIndHigh + 1u) {
				// There is a hole
				it = nonRecoveringFields.insert(it,
						std::pair<std::string, Field>("",
								Field(prevIndHigh + 1u,
										it->second.getIndLow() - 1u, "", NA)));
				++it;
			}
			prevIndHigh = it->second.getIndHigh();
			++it;
		}
		// Checking if fields end with an empty space (MSB side)
		if (prevIndHigh < (regSize - 1u)) {
			nonRecoveringFields.push_back(
					std::pair<std::string, Field>("",
							Field(prevIndHigh + 1u, regSize - 1u, "", NA)));
		}

		// Filling table
		for (std::vector<std::pair<std::string, Field>>::const_reverse_iterator rit =
				nonRecoveringFields.rbegin(); rit != nonRecoveringFields.rend();
				++rit) {
			Fields::fieldsmap_t::const_iterator itTmp = fields.find(
					rit->second.getIndLow(), rit->second.getIndHigh());
			if (itTmp != fields.cend()) {
				t.add(itTmp->first);
			} else {
				t.add(std::string(""));
			}
		}
		t.endOfRow();
		for (std::vector<std::pair<std::string, Field>>::const_reverse_iterator rit =
				nonRecoveringFields.rbegin(); rit != nonRecoveringFields.rend();
				++rit) {
			t.add(
					this->getValue()(rit->second.getIndLow(),
							rit->second.getIndHigh()));
		}
		t.endOfRow();
	} else {
		t.add(this->toString());
		t.endOfRow();
	}

	ret << t;
	return ret.str();
}

std::string Register::toString() const {
	return data.toString();
}

std::ostream& operator <<(std::ostream &strm, const Register &reg) {
	return strm << reg.toString();
}

void Register::updateMasks() {
	if (mode == NA) {
		readMask = ~BitVector(this->getSize(), 0u);
		writeMask = ~BitVector(this->getSize(), 0u);
		for (Fields::fieldsmap_t::const_iterator it = fields.cbegin();
				it != fields.cend(); ++it) {
			std::size_t indLow(it->second.getIndLow());
			std::size_t indHigh(it->second.getIndHigh());
			hvrwmode_t effectiveMode;
			effectiveMode = it->second.getRWMode();
			std::size_t nOnes(indHigh - indLow + std::size_t(1u));
			BitVector maskTmp(nOnes, 0u);
			maskTmp = ~maskTmp;
			BitVector mask(this->getSize(), maskTmp);
			mask <<= static_cast<hvuint32_t>(indLow);
			if (effectiveMode == RW) {
				readMask &= ~mask;
				readMask |= mask;
				writeMask &= ~mask;
				writeMask |= mask;
			} else if (effectiveMode == RO) {
				readMask &= ~mask;
				readMask |= mask;
				writeMask &= ~mask;
			} else if (effectiveMode == WO) {
				readMask &= ~mask;
				writeMask &= ~mask;
				writeMask |= mask;
			} else {
				HV_ERR("This line should not have been reached.");
				exit(EXIT_FAILURE);
			}

		}
	}
}

hvcbID_t Register::getUniqueID() {
	return cbIDCpt++;
}

}
// namespace reg
}// namespace hv

