#include <iostream>
#include <HVRegister>

using namespace ::hv::common;
using namespace ::hv::reg;

int sc_main(int argc, char* argv[]) {
	Register reg(32, "myReg", "A great register", RW);
	std::cout << reg.getInfo() << std::endl;
	return 0;
}
