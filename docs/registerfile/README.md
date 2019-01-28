# RegisterFile Class - Starting Guide

## General View

<img src="img/registerfile.png" width="500">

## Description

RegisterFile class maps registers (from Register class) and other register files (from RegisterFile class) into a single address space. Memory coherence is ensured in the sense that two elements cannot occupy common memory spaces, and in the sense that elements can only start to aligned addresses (see [Address Alignment and Compactness](#Address-Alignment-and-Compactness)).

## Accessing RegisterFile

RegisterFile class is declared in `registerfile.h` and is part of namespace `::hv::reg`. RegisterFile class uses Register class which is part of the same namespace. In the rest of this file, we suppose that the following directives are included in the code:

```cpp
#include "registerfile.h"
using namespace ::hv::common;
using namespace ::hv::reg;
```

## Address Alignment and Compactness

Before starting, it is important to understand the concept of address alignment and compactness.

In a RegisterFile, all addresses are _aligned_ to a certain value, which can be default or specified. The alignment is specified in bytes and must be a power of 2, and a child RegisterFile must have the same alignment as its parent(s).

Alignment value determines:

- The addresses where a given element can be inserted, and
- The behavior of automatic address resolution when an element is inserted with no address specified.

Next sub-sections detail these specific behaviors.

The alignment is an unsigned integer specified at RegisterFile's creation as third argument, e.g:

```cpp
// Instanciation of 4-byte aligned RegisterFile
RegisterFile regFile("Name", "Description", 4);
```

### Case 1: No Alignment Specified (default behavior)

When no alignment is specified, default alignment is applied.

- When an address is specified for the insertion of an element, the address must be a multiple of the size of the element to insert, rounded to nearest superior power of 2. A few examples:
  - 32-bit (4-byte) registers can only be inserted to any hexadecimal address ending by 0, 4, 8 and C,
  - 24-bit (3-byte) registers can only be inserted to the exact same addresses than 32-bit registers (which correspond to the nearest superior power of 2,)
  - 64-bit (8-byte) registers can only be inserted to any hexadecimal address ending by 0, 8,
  - A 23-bytes register file can only be inserted to an address which is a multiple of 32 (which is the nearest superior power of 2).
- When no address is specified, the address is resolved taking the first free aligned address (following the rule above) at the end of occupied space.

Thus, when no alignment is specified, the default behavior can lead to important empty spaces, especially when inserting large elements.

This behavior mathematically implies that any X-bytes register is always aligned relatively to its absolute address, even inside a deep hierarchy of register files.

### Case 2: Alignment Specified

When an alignment is specified, the behavior is as follows:

- When an address is specified for the insertion of an element, the address must be a multiple of the specified alignment, whatever its size. E.g, if an X-bytes element is inserted to a RegisterFile with a 4-byte alignment, it can be inserted to any hexadecimal address ending by 0, 4, 8 and C.
- When no address is specified, the element is inserted to the first free aligned address at the end of occupied space.

In other words, when alignment is specified, the default layout is _compact_. Indeed, when no address is specified during insertion, there are no possible free aligned addresses between 2 occupied addresses. Moreover, as all alignments are forced to be the same for all RegisterFiles in a same hierarchy, there can't be any unaligned element, relatively to any RegisterFile in the upstream hierarchy.

## Absolute and Relative Address

A RegisterFile references its elements with their addresses related to its own address 0. In the case we have a RegisterFile `regFileIn` contained at the address `addrRegFileIn` of a direct parent RegisterFile `regFileParent`, then any element `eltIn` of `regFileIn` at address `addrEltIn` has a relative address of `addrRegFileIn + addrEltIn` relatively to `regFileParent`.

In general, the address of an element of a sub-child relatively to one of its parent is equal to the address of this element relatively to this sub-child added to the relative offsets of all the intermediates parents.

An address is told _absolute_ when it is given relatively to the top parent.

## A Basic Example

This section develops a demonstration of RegisterFile instantiation and manipulation. All subsequent pieces of code presented here must be seen as a single monolithic code block.

First, let's create a top RegisterFile with an alignment of 4-bytes.

```cpp
// Top register file creation
RegisterFile topRF("Top Register File", "This is the top register file", 4);
```

As the alignment is defined, the alignment rules are the ones of [this section](#Case-2:-Alignment-Specified).

Now, let's create some registers to insert to this RegisterFile.

```cpp
// Registers creation
Register<32> r1("Register 1", "This is register 1 of topRF.", READ);
Register<32> r2("Register 2", "This is register 2 of topRF.", WRITE);
Register<24> r3("Register 3", "This is register 3 of topRF.", READ_WRITE);
```

Let's now add these registers to our top RegisterFile.

```cpp
// Adding registers to top register file
topRF.addRegister(r1);
topRF.addRegister(r2);
topRF.addRegister(r3);
```

Let's display our top RegisterFile's current state:

```cpp
// Displaying top register file information
std::cout << topRF.getInfo() << std::endl;
```

Which displays:

```bash
Register file:
	Name: Top Register File
	Description: This is the top register file
+------------+----+-------+----------+----------------------------+
|Rel. address|Type|Size   |Name      |Description                 |
+------------+----+-------+----------+----------------------------+
|0x0 -> 0x3  |Reg.|4 bytes|Register 1|This is register 1 of topRF.|
+------------+----+-------+----------+----------------------------+
|0x4 -> 0x7  |Reg.|4 bytes|Register 2|This is register 2 of topRF.|
+------------+----+-------+----------+----------------------------+
|0x8 -> 0xA  |Reg.|3 bytes|Register 3|This is register 3 of topRF.|
+------------+----+-------+----------+----------------------------+
```

As expected, our registers were successively inserted at the end of occupied space, respecting 4-byte alignment.

If we now check the value of `r1` directly reading from it, and getting it from `topRF`:

```cpp
std::cout << "Before - (r1):" << r1 << std::endl;
std::cout << "Before - (r1 through topRF):" << topRF.getRegister<32>("Register 1") << std::endl;
```

We get:

```bash
Before - (r1):               00000000000000000000000000000000
Before - (r1 through topRF): 00000000000000000000000000000000
```

Which is the default value of uninitialized registers (see [Register starting-up guide](../register/README.md#A-Basic-Example)). Let's now directly modify its value.

```cpp
r1 = 0x0F0F0F0F;
```

If we now check `r1` value as previously:

```cpp
std::cout << "After - (r1):" << r1 << std::endl;
std::cout << "After - (r1 through topRF):" << topRF.getRegister<32>("Register 1") << std::endl;
```

We get:

```bash
After - (r1):               00001111000011110000111100001111
After - (r1 through topRF): 00001111000011110000111100001111
```

Indeed, `addRegister()` method adds a Register **by reference**, which means there is a single instance of `r1` in memory. `getRegister<W>()` method returns a reference of type `Register<W>&` to the register sought. Be careful, using non-template version of `getRegister()` returns a reference of type `RegisterIf&`, and therefore only methods implemented in register's interface may be used.

Let's now create a new register `r4` and add it by copy at address `0x10`.

```cpp
Register<64> r4("Register 4", "This is register 4 of topRF.", READ_WRITE);
topRF.addRegisterCopy(0x10, r4);
```

If we now check the value of `r4` directly and the value of `r4` through `topRF`, we get:

```bash
Before - (r4):               0000000000000000000000000000000000000000000000000000000000000000
Before - (r4 through topRF): 0000000000000000000000000000000000000000000000000000000000000000
```

Now let's modify `r4`.

```cpp
r4 = 0x5555555555555555;
```

Now when checking the values:

```bash
After - (r4):               0101010101010101010101010101010101010101010101010101010101010101
After - (r4 through topRF): 0000000000000000000000000000000000000000000000000000000000000000
```

As expected, only our local version of `r4` was modified and not the copy inserted to `topRF`.

If we now check `topRF` state, we get:

```bash
Register file:
	Name: Top Register File
	Description: This is the top register file
+------------+----+-------+----------+----------------------------+
|Rel. address|Type|Size   |Name      |Description                 |
+------------+----+-------+----------+----------------------------+
|0x0 -> 0x3  |Reg.|4 bytes|Register 1|This is register 1 of topRF.|
+------------+----+-------+----------+----------------------------+
|0x4 -> 0x7  |Reg.|4 bytes|Register 2|This is register 2 of topRF.|
+------------+----+-------+----------+----------------------------+
|0x8 -> 0xA  |Reg.|3 bytes|Register 3|This is register 3 of topRF.|
+------------+----+-------+----------+----------------------------+
|0x10 -> 0x17|Reg.|8 bytes|Register 4|This is register 4 of topRF.|
+------------+----+-------+----------+----------------------------+
```

Our 64-bit Register was inserted to address `0x10`. `topRF` is now not compact anymore as aligned address `0xC` is free but not at the end of occupied space.

A register can also directly be created from a RegisterFile instead of copying an external register. E.g:

```cpp
topRF.createRegister<32>(0x100, "Register 5", "This is register 5 of topRF.", READ);
```

creates a 32-bit Register at address `0x100`.

```bash
+--------------+----+-------+----------+----------------------------+
|Rel. address  |Type|Size   |Name      |Description                 |
+--------------+----+-------+----------+----------------------------+
|0x0 -> 0x3    |Reg.|4 bytes|Register 1|This is register 1 of topRF.|
+--------------+----+-------+----------+----------------------------+
|0x4 -> 0x7    |Reg.|4 bytes|Register 2|This is register 2 of topRF.|
+--------------+----+-------+----------+----------------------------+
|0x8 -> 0xA    |Reg.|3 bytes|Register 3|This is register 3 of topRF.|
+--------------+----+-------+----------+----------------------------+
|0x10 -> 0x17  |Reg.|8 bytes|Register 4|This is register 4 of topRF.|
+--------------+----+-------+----------+----------------------------+
|0x100 -> 0x103|Reg.|4 bytes|Register 5|This is register 5 of topRF.|
+--------------+----+-------+----------+----------------------------+
```

Now, let's create an internal register file called `intRF1` to be inserted to `topRF`. As stated in [Address Alignment and Compactness section](#Address-Alignment-and-Compactness), the alignment of register files in a same hierarchy must be the same, which is 4 bytes in our case.

```cpp
// Creating internal register file
RegisterFile intRF1("InternalRegisterFile", "This is an internal reg. file.", 4);
```

Let's now create registers inside this register file:

```cpp
// Creating registers inside intRF1
intRF1.createRegister<16>("IntRF1 Register 1", "This is register 1 of intRF1.", WRITE);
intRF1.createRegister<32>(0xC,"IntRF1 Register 2", "This is register 2 of intRF1.", READ_WRITE);
intRF1.createRegister<32>("IntRF1 Register 3", "This is register 3 of intRF1.", READ_WRITE);
```

Displaying the info of `intRF1` returns:

```bash
Register file:
	Name: InternalRegisterFile1
	Description: This is an internal reg. file.
+------------+----+-------+-----------------+-----------------------------+
|Rel. address|Type|Size   |Name             |Description                  |
+------------+----+-------+-----------------+-----------------------------+
|0x0 -> 0x1  |Reg.|2 bytes|IntRF1 Register 1|This is register 1 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0xC -> 0xF  |Reg.|4 bytes|IntRF1 Register 2|This is register 2 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0x10 -> 0x13|Reg.|4 bytes|IntRF1 Register 3|This is register 3 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
```

As you already know, we can add registers by reference. Therefore, if we add the following instruction:

```cpp
// Adding "Register 4" from topRF to intRF1
intRF1.addRegister(0x24, topRF.getRegister<24>("Register 4"));
```

Then *Register 4* of `topRF` is added to `intRF1` by reference. Both registers are therefore actually referring to a same single instance, contained in `topRF`. Displaying `intRF1` info returns:

```bash
Register file:
	Name: InternalRegisterFile1
	Description: This is an internal reg. file.
+------------+----+-------+-----------------+-----------------------------+
|Rel. address|Type|Size   |Name             |Description                  |
+------------+----+-------+-----------------+-----------------------------+
|0x0 -> 0x1  |Reg.|2 bytes|IntRF1 Register 1|This is register 1 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0xC -> 0xF  |Reg.|4 bytes|IntRF1 Register 2|This is register 2 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0x10 -> 0x13|Reg.|4 bytes|IntRF1 Register 3|This is register 3 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0x24 -> 0x2B|Reg.|8 bytes|Register 4       |This is register 4 of topRF. |
+------------+----+-------+-----------------+-----------------------------+
```

Let's modify the register at address `0x24` of `intRF1`.

```cpp
intRF1.getRegister<24>(0x24) = 0xFFFFFF;
```

Let's display the value *Register 4* retrieved from both register files:

```cpp
std::cout << "Register 4 from topRF:  " << topRF.getRegister<24>("Register 4") << std::endl;
std::cout << "Register 4 from intRF1: " << topRF.getRegister<24>("Register 4") << std::endl;
```

We get as expected:

```bash
Register 4 from topRF:  111111111111111111111111
Register 4 from intRF1: 111111111111111111111111
```

Now, let's insert `intRF1` and the end of occupied space of `topRF`.

```cpp
// Inserting intRF1 into topRF
topRF.addRegisterFile(intRF1);
```

If we now display info about `topRF`, we get:

```bash
Register file:
	Name: Top Register File
	Description: This is the top register file
+--------------+---------+--------+---------------------+------------------------------+
|Rel. address  |Type     |Size    |Name                 |Description                   |
+--------------+---------+--------+---------------------+------------------------------+
|0x0 -> 0x3    |Reg.     |4 bytes |Register 1           |This is register 1 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x4 -> 0x7    |Reg.     |4 bytes |Register 2           |This is register 2 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x8 -> 0xA    |Reg.     |3 bytes |Register 3           |This is register 3 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x10 -> 0x17  |Reg.     |8 bytes |Register 4           |This is register 4 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x100 -> 0x103|Reg.     |4 bytes |Register 5           |This is register 5 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x104 -> 0x12F|Reg. File|44 bytes|InternalRegisterFile1|This is an internal reg. file.|
+--------------+---------+--------+---------------------+------------------------------+
```

Before inserting `intRF`, its size was undefined and (nearly) infinite. When inserted, register file size is defined forever and the register file is locked: no child can then be inserted, register or register file. If not explicitly given, the size is set to the minimal size containing the whole register file. In previous `intRF1`, space being occupied until relative address `0x2B` (43 in decimal representation), the default size is 44 bytes, starting address being always `0x0`.

Let's consider a RegisterFile called `intRF2` with the following structure:

```bash
Register file:
	Name: InternalRegisterFile2
	Description: This is an internal reg. file.
+------------+----+-------+-----------------+----------------------------+
|Rel. address|Type|Size   |Name             |Description                 |
+------------+----+-------+-----------------+----------------------------+
|0xC -> 0xF  |Reg.|4 bytes|IntRF2 Register 1|This is register 1 of intRF2|
+------------+----+-------+-----------------+----------------------------+
|0x10 -> 0x13|Reg.|4 bytes|IntRF2 Register 2|This is register 2 of intRF2|
+------------+----+-------+-----------------+----------------------------+
|0x14 -> 0x17|Reg.|4 bytes|IntRF2 Register 3|This is register 3 of intRF2|
+------------+----+-------+-----------------+----------------------------+
|0x18 -> 0x1B|Reg.|4 bytes|IntRF2 Register 4|This is register 4 of intRF2|
+------------+----+-------+-----------------+----------------------------+
```

The minimal size of this register file is 28 bytes. If we want to insert this RegisterFile in free space of `topRF` starting at address `0x20` with a size of 36 bytes, we can do it the following way:

```cpp
topRF.addRegisterFile(0x20, intRF2, 36);
```

and finally `topRF` is:

```bash
Register file:
	Name: Top Register File
	Description: This is the top register file
+--------------+---------+--------+---------------------+------------------------------+
|Rel. address  |Type     |Size    |Name                 |Description                   |
+--------------+---------+--------+---------------------+------------------------------+
|0x0 -> 0x3    |Reg.     |4 bytes |Register 1           |This is register 1 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x4 -> 0x7    |Reg.     |4 bytes |Register 2           |This is register 2 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x8 -> 0xA    |Reg.     |3 bytes |Register 3           |This is register 3 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x10 -> 0x17  |Reg.     |8 bytes |Register 4           |This is register 4 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x20 -> 0x43  |Reg. File|36 bytes|InternalRegisterFile2|This is an internal reg. file.|
+--------------+---------+--------+---------------------+------------------------------+
|0x100 -> 0x103|Reg.     |4 bytes |Register 5           |This is register 5 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x104 -> 0x12F|Reg. File|44 bytes|InternalRegisterFile1|This is an internal reg. file.|
+--------------+---------+--------+---------------------+------------------------------+
```

Now our register files hierarchy is complete, we can view recursive info about every elements in `topRF` hierarchy calling `getRecursiveInfo()` method. E.g:

```cpp
std::cout << topRF.getRecursiveInfo() << std::endl;
```

displays:

```cpp
############## Register files details - level 0 ##############
Register file:
	Name: Top Register File
	Description: This is the top register file
+--------------+---------+--------+---------------------+------------------------------+
|Rel. address  |Type     |Size    |Name                 |Description                   |
+--------------+---------+--------+---------------------+------------------------------+
|0x0 -> 0x3    |Reg.     |4 bytes |Register 1           |This is register 1 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x4 -> 0x7    |Reg.     |4 bytes |Register 2           |This is register 2 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x8 -> 0xA    |Reg.     |3 bytes |Register 3           |This is register 3 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x10 -> 0x17  |Reg.     |8 bytes |Register 4           |This is register 4 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x20 -> 0x43  |Reg. File|36 bytes|InternalRegisterFile2|This is an internal reg. file.|
+--------------+---------+--------+---------------------+------------------------------+
|0x100 -> 0x103|Reg.     |4 bytes |Register 5           |This is register 5 of topRF.  |
+--------------+---------+--------+---------------------+------------------------------+
|0x104 -> 0x12F|Reg. File|44 bytes|InternalRegisterFile1|This is an internal reg. file.|
+--------------+---------+--------+---------------------+------------------------------+
############## Register files details - level 1 ##############
Register file:
	Name: InternalRegisterFile2
	Description: This is an internal reg. file.
+------------+----+-------+-----------------+----------------------------+
|Rel. address|Type|Size   |Name             |Description                 |
+------------+----+-------+-----------------+----------------------------+
|0xC -> 0xF  |Reg.|4 bytes|IntRF2 Register 1|This is register 1 of intRF2|
+------------+----+-------+-----------------+----------------------------+
|0x10 -> 0x13|Reg.|4 bytes|IntRF2 Register 2|This is register 2 of intRF2|
+------------+----+-------+-----------------+----------------------------+
|0x14 -> 0x17|Reg.|4 bytes|IntRF2 Register 3|This is register 3 of intRF2|
+------------+----+-------+-----------------+----------------------------+
|0x18 -> 0x1B|Reg.|4 bytes|IntRF2 Register 4|This is register 4 of intRF2|
+------------+----+-------+-----------------+----------------------------+
############## Register files details - level 1 ##############
Register file:
	Name: InternalRegisterFile1
	Description: This is an internal reg. file.
+------------+----+-------+-----------------+-----------------------------+
|Rel. address|Type|Size   |Name             |Description                  |
+------------+----+-------+-----------------+-----------------------------+
|0x0 -> 0x1  |Reg.|2 bytes|IntRF1 Register 1|This is register 1 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0xC -> 0xF  |Reg.|4 bytes|IntRF1 Register 2|This is register 2 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0x10 -> 0x13|Reg.|4 bytes|IntRF1 Register 3|This is register 3 of intRF1.|
+------------+----+-------+-----------------+-----------------------------+
|0x24 -> 0x2B|Reg.|8 bytes|Register 4       |This is register 4 of topRF. |
+------------+----+-------+-----------------+-----------------------------+
##############################################################
```

Here, all addresses are given relative to the considered register file. You can get all elements in the hierarchy in absolute address (see the difference in [Absolute and Relative Address section](#Absolute-and-Relative-Address)) this way:

```cpp
std::cout << topRF.getFlattenedInfo() << std::endl;
```

displaying:

```bash
Register file:
	Name: Top Register File
	Description: This is the top register file
+--------------+----+-------+-----------------+-----------------------------+
|Rel. address  |Type|Size   |Name             |Description                  |
+--------------+----+-------+-----------------+-----------------------------+
|0x0 -> 0x3    |Reg.|4 bytes|Register 1       |This is register 1 of topRF. |
+--------------+----+-------+-----------------+-----------------------------+
|0x4 -> 0x7    |Reg.|4 bytes|Register 2       |This is register 2 of topRF. |
+--------------+----+-------+-----------------+-----------------------------+
|0x8 -> 0xA    |Reg.|3 bytes|Register 3       |This is register 3 of topRF. |
+--------------+----+-------+-----------------+-----------------------------+
|0x10 -> 0x17  |Reg.|8 bytes|Register 4       |This is register 4 of topRF. |
+--------------+----+-------+-----------------+-----------------------------+
|0x2C -> 0x2F  |Reg.|4 bytes|IntRF2 Register 1|This is register 1 of intRF2 |
+--------------+----+-------+-----------------+-----------------------------+
|0x30 -> 0x33  |Reg.|4 bytes|IntRF2 Register 2|This is register 2 of intRF2 |
+--------------+----+-------+-----------------+-----------------------------+
|0x34 -> 0x37  |Reg.|4 bytes|IntRF2 Register 3|This is register 3 of intRF2 |
+--------------+----+-------+-----------------+-----------------------------+
|0x38 -> 0x3B  |Reg.|4 bytes|IntRF2 Register 4|This is register 4 of intRF2 |
+--------------+----+-------+-----------------+-----------------------------+
|0x100 -> 0x103|Reg.|4 bytes|Register 5       |This is register 5 of topRF. |
+--------------+----+-------+-----------------+-----------------------------+
|0x104 -> 0x105|Reg.|2 bytes|IntRF1 Register 1|This is register 1 of intRF1.|
+--------------+----+-------+-----------------+-----------------------------+
|0x110 -> 0x113|Reg.|4 bytes|IntRF1 Register 2|This is register 2 of intRF1.|
+--------------+----+-------+-----------------+-----------------------------+
|0x114 -> 0x117|Reg.|4 bytes|IntRF1 Register 3|This is register 3 of intRF1.|
+--------------+----+-------+-----------------+-----------------------------+
|0x128 -> 0x12F|Reg.|8 bytes|Register 4       |This is register 4 of topRF. |
+--------------+----+-------+-----------------+-----------------------------+
```

Now our main view is built and verify, we just have to access and manipulate our registers. To learn how to access them, go to [next section](#Register-and-RegisterFile-Access-Details).

To learn how to manipulate registers and the many embedded possibilities, you can follow [Register starting guide](../register/README.md).

## Register and RegisterFile Access Details

In a RegisterFile, internal RegisterFiles can be accessed with `operator []` by name or address. Registers are accessed using `operator ()` by name or address. However, their behaviors are different:

- A RegisterFile can be accessed by name or address only from its direct RegisterFile container.
- A Register can be accessed by name only from its direct RegisterFile container.
- A Register can be accessed by address from a container of any higher level.

Let's focus on the following structure, where RegisterFiles are in capital letters and Registers in lowercase. X is the instance name of register named "X". All registers are consecutive in memory and 1-byte long for easier understanding. The first address column represents address relatively to A, the second relatively to B and the third relatively to C.

```bash
0  A (top)
0  |__ a1
1  |__ a2
2  |__ B
2  | 0 |__ b1
3  | 1 |__ b2
4  | 2 |__ b3
5  | 3 |__ C
5  | 3 | 0 |__ c1
6  | 4 | 1 |__ c2
7  | 5 |__ b3
8  |__ a3
```
You can notice that RegisterFiles and their first Register have the same address, which is consistent with our hypothesis.
Here are all the possible direct accesses to register `c2`:

```cpp
// 3-step accesses
    // Only by name
    A["B"]["C"]("c2");
    // By name and address combinations
    A["B"]["C"](1);
    A["B"][3]("c2");
    A["B"][3](1);
    A[2]["C"]("c2");
    A[2]["C"](1);
    A[2][3]("c2");
    // Only by address
    A[2][3](1);
// 2-step accesses
	A["B"](4);
    A[2](4);
// 1-step access
	A(6);
```

When accessing with 3 steps, all combinations name/addresses are possible. However, if you want to access `c2` from higher-level RegisterFiles, you have to access it by relative address (which is absolute address for top-level RegisterFile).

Important notes:

- `operator []` is an alias for method `getRegisterFile()` and `operator ()` is an alias for method `getRegister()`.
- `operator()` returns a `RegisterIf&` reference and therefore, only methods implemented in register's interface are accessible from this reference. To get a `Register<W>&` reference, `getRegister<W>()` method has to be used.

Now it's up to you. Be Hiventive!