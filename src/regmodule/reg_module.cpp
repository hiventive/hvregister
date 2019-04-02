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

RegModule<8> mRegModule8("MyRegModule8");
RegModule<16> mRegModule16("MyRegModule16");
RegModule<32> mRegModule32("MyRegModule32");
RegModule<64> mRegModule64("MyRegModule64");
RegModule<128> mRegModule128("MyRegModule128");

} // namespace hv
} // namespace module
