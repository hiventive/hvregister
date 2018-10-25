/**
 * @file reg_module.cpp
 * @author Benjamin Barrois <benjamin.barrois@hiventive.com>
 * @date May, 2018
 * @copyright Copyright (C) 2018, Hiventive.
 *
 * @brief Register-style module
 */

#include "reg_module.h"

namespace hv {
namespace reg {

RegModule<8> mRegModule8("myRegModule8");
RegModule<16> mRegModule16("myRegModule16");
RegModule<32> mRegModule32("myRegModule32");
RegModule<64> mRegModule64("myRegModule64");
RegModule<128> mRegModule128("myRegModule128");

} // namespace hv
} // namespace module
