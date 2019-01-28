# Register Class - Starting Guide

## General View

<img src="img/register.png" width="500">

## Description

Register class is the base class used to represent hardware registers with arbitrary bit width. It was designed for intuitive fields creation and manipulation. By default, numbers are represented using BitVector class, which allows good computation performance while keeping high flexibility.

## Accessing Register

Register class is declared in `register.h` and is part of namespace `::hv::reg`. By default, register uses BitVector which is part of namespace `::hv::common`. In the rest of this file, we suppose that the following directives are included in the code.

```cpp
#include <register/register.h>
using namespace ::hv::common;
using namespace ::hv::reg;
```

## A Basic How-To

This section develops a demonstration of Register syntax and manipulation. All pieces of code presented here must be seen as a single monolithic code block.

### Register Creation

Let's create your first register. It will be a 32-bit register called *MyRegister*, with a description telling that it is your register. This register will be in Read/Write mode, and with a reset value of 0.

```cpp
// Register declaration
Register reg(32, "MyRegister", "This is my register.", RW, 0);
```

We can get details about our register any time using `getInfo()` method:

```cpp
// Displaying info
std::cout << reg.getInfo() << std::endl;
```

In our case, this displays:

```bash
##################################################
Register name:
	MyRegister
Register description:
	This is my register.
Read/Write mode:
	Read-Write
Reset value:
	0x00000000
Read mask value:
	0xFFFFFFFF
Write mask value:
	0xFFFFFFFF

#### Register general view ####
+--------------------------------+
|00000000000000000000000000000000|
+--------------------------------+

#### Register value ####
	Bin: 00000000000000000000000000000000
	Hex: 0x00000000

#### Fields details ####
< No fields defined >
##################################################
```

The first fields give the main static details about our register: *name*, *description*, *mode*, *reset value* and *read/write masks*. The read and write masks are applied when using *read(.)* and *write(.)* methods. These masks are auto-resolved. In our case, the only detail we gave about our register is that it is in *RW* mode. Therefore, both masks are filled with 1s.

The second field gives a general register view, including its current value. When fields are defined (see [Field Creation section](#Field-Creation)), they appear on this view.

The third field gives our register's current value displayed in binary and hexadecimal representations.

Finally, fields details are given (see [Field Creation section](#Field-Creation)).

### Field Creation

Our register can be manipulated at field-level. To do this, we must first create fields giving their position and name.

Let's create a field called *Field1* defined from bit 1 to bit 7.

```cpp
// Creating field
reg.createField("Field1", 7, 1, "My first field.");
```

Now if we display register info, we get:

```bash
##################################################
Register name:
	MyRegister
Register description:
	This is my register.
Read/Write mode:
	Read-Write
Reset value:
	0x00000000
Read mask value:
	0xFFFFFFFF
Write mask value:
	0xFFFFFFFF

#### Register general view ####
+------------------------+-------+-+
|                        |Field1 | |
+------------------------+-------+-+
|000000000000000000000000|0000000|0|
+------------------------+-------+-+

#### Register value ####
	Bin: 00000000000000000000000000000000
	Hex: 0x00000000

#### Fields details ####
Field name:
	Field1
Field description:
	My first field.
Field position:
	7 <- 1
-----------------------
##################################################
```

You can declare as many fields as you want. They can even recover, but in this case they won't all display in *Register general view* section when displaying. However, all fields of a same register must have different names. Here a a few other examples of field creation. *Field2* corresponds to a single-bit field. *Field3* has no description.

```cpp
// Creating more fields
reg.createField("Field2", 9, "My second field.");
reg.createField("Field3", 31, 20);
```

```bash
##################################################
Register name:
	MyRegister
Register description:
	This is my register.
Read/Write mode:
	Read-Write
Reset value:
	0x00000000
Read mask value:
	0xFFFFFFFF
Write mask value:
	0xFFFFFFFF

#### Register general view ####
+------------+----------+------+-+-------+-+
|Field3      |          |Field2| |Field1 | |
+------------+----------+------+-+-------+-+
|000000000000|0000000000|0     |0|0000000|0|
+------------+----------+------+-+-------+-+

#### Register value ####
	Bin: 00000000000000000000000000000000
	Hex: 0x00000000

#### Fields details ####
Field name:
	Field1
Field description:
	My first field.
Field position:
	7 <- 1
-----------------------
Field name:
	Field2
Field description:
	My second field.
Field position:
	9
-----------------------
Field name:
	Field3
Field description:
	<no available description>
Field position:
	31 <- 20
-----------------------
##################################################
```

Now you know how to create registers and fields, let's see how to manipulate their values.

### Register Value Manipulation

Our registers were made for intuitive and efficient value manipulation. It is based on **BitVector** class for number representation (see **BitVector**'s documentation) and thus it comes with very handy manipulation methods.

Let's create a new register and its fields:

```cpp
// Register declaration
Register reg2(16, "myNewRegister", "This is my new register", RW);

// Fields creation
reg2.createField("f1", 3, 0);
reg2.createField("f2", 7, 4);
reg2.createField("f3", 11, 8);
reg2.createField("f4", 15, 12);

std::cout << reg2.getInfo(0) << std::endl;
```

**Note:** `getInfo(0)` displays level-0 information (only register general view section). `getInfo()` is an alias for `getInfo(1)` which displays level-1 information. The previous code displays:

```bash
##################################################
+----+----+----+----+
|f4  |f3  |f2  |f1  |
+----+----+----+----+
|0000|0000|0000|0000|
+----+----+----+----+
##################################################
```

**Note:** No default register value was given. Overall default reset value is 0.

Let's set a new value to our register. From this point, all code blocks will be followed by the new register value displayed using a call to method `getInfo(0)`.

```cpp
// Modifying register value
reg2 = 0x0F0F;
```

```bash
+----+----+----+----+
|f4  |f3  |f2  |f1  |
+----+----+----+----+
|0000|1111|0000|1111|
+----+----+----+----+
```

Now let's modify only *f1*'s value.

```cpp
// Modifying f1's value
reg2("f1") = 5;
```

```bash
+----+----+----+----+
|f4  |f3  |f2  |f1  |
+----+----+----+----+
|0000|1111|0000|0101|
+----+----+----+----+
```

Let's now modify *f4*'s value with *f1*'s.

```cpp
// Setting f4's value with f1's
reg2("f4") = reg2("f1");
```

```bash
+----+----+----+----+
|f4  |f3  |f2  |f1  |
+----+----+----+----+
|0101|1111|0000|0101|
+----+----+----+----+
```

Actually you can do quite everything you want. On the following example, bits 3 down to 1 of `f2` are set to bits 2 down to 0 of the result of the binary OR of *f1* and the binary negation of `f4`.

```cpp
// Doing whatever you want
reg2("f2")(3,1) = (reg2("f1") | ~reg2("f4"))(2,0);
```

```bash
+----+----+----+----+
|f4  |f3  |f2  |f1  |
+----+----+----+----+
|0101|1111|1110|0101|
+----+----+----+----+
```

Indeed, you can combine any vector/bit selection using operators `(.,.)` and `[.]` with field selection and any operation available in **BitVector** (all binary operations, shifting, concatenation), and put the result in any other selection (whole register, whole field, register selection, field selection...). Everything combination is just possible. You can even interoperate blindly with native types:

```cpp
// Interoperability example
hvuint32_t x = (reg2(15,7) & 0xF0) + reg2("f1");
std::cout << std::hex << "x = 0x" << x << std::endl;
```

which displays:

```bash
x = 0x505 // Binary value: 010100000101
```

In this previous example, we performed a binary AND on the MSB half of *reg2* and an implicit integer (0xF0). This results in `01010000`. Then, this result is concatenated with `f1` which is `0101`. Then, this result is stored in a native unsigned integer and displayed.

### Registering Read/Write Callbacks

For most user-accessible registers, reading from or writing to registers trigger reactions. E.g, timers all have a control register which allows to modify clock speed or the value to reach before triggering an IRQ. To avoid creating new classes inheriting from Register to add custom functional behavior, Hiventive's Register class allows you to register 4 kinds of callbacks:

- Pre-read callbacks
- Post-read callbacks
- Pre-write callbacks
- Post-write callbacks

If you don't know what a callback is or if you just want to see some simple examples of callbacks, please read our [Callback Starting Guide](#TODO-add-ref).

Every register you create using our framework is able to run any of these 4 kinds of callbacks. These callbacks must be registered after register creation using **Register**'s methods `registerPreReadCallback(.)`, `registerPostReadCallback(.)`, `registerPreWriteCallback(.)` and `registerPostReadCallback(.)`.

When `read(.)` (resp. `write(.)`) method is called, all pre-read (resp. pre-write) callbacks are called in the same order they were registered. If all pre-read (resp. pre-write) callbacks returned `true`, the value is read (resp. written) and all post-read (resp. post-write) callbacks are called. Post-read (resp. post-write) callbacks have no return value.

All pre-read callbacks carry the value of the register before reading (reading can modify the register value if you have decided to) and all pre-write callbacks carry the value of the register after reading.

All pre-write callbacks and post-write callbacks carry both the value of the register before writing and after writing.

All 4 kinds of callbacks also carry a handle of the register so all its methods can be accessed from inside the callback though the callbacks are created outside register scope.

All previous information and handles are carried in objects of type `Register::read_event_t` and `Register::write_event_t`. They need to be the only argument of your callbacks, but you don't need to do anything with it, their transmission from callback to callback is managed by Register class itself.

Now I managed to give you a headache, let's see how it works with a simple example.

Let's consider the following structure:

```cpp
struct fooStruct {
    // Some register
    Register reg;

    // Some class method we can register as pre-read callback
    bool checkSomethingBeforeReading(
        const Register::read_event_t &ev) const {
        std::cout << ev.rh.getName() << " - Read event value: " << ev.value
            << std::endl;
        std::cout << reg.getName() << " - My current value: " << reg
            << std::endl << std::endl;
        // Returning true to allow reading
        return true;
    }

    // fooStruct constructor
    fooStruct(const std::string &name) :
    reg(32, name, "", RW) {
    }

};
```

This structure contains a **Register** and a pre-read callback-compliant method. In this method, the value contained by the read event is displayed, as well as the value of the register contained in the hypothetical instance of fooStruct. This method blindly returns true to tell the hypothetical next pre-read callback "ok, you can go on".

Now let's imagine we have, outside of this class scope, the following function implementation:

```cpp
// Some function we can register as post-read callback
void displayValuesAfterWriting(const Register::write_event_t &ev) {
	std::cout << ev.rh.getName() << " - Write event old value: " << ev.oldValue
			<< std::endl;
	std::cout << ev.rh.getName() << " - Write event new value: " << ev.newValue
			<< std::endl << std::endl;
}
```

This function is post-read callback-compliant. It just displays the old value of the register represented by the event before writing, and the new value.

For now, both the method and the function are totally uncorrelated from any register instance, they only have a declarative status.

Now, let's instantiate a pair of `fooStruct`.

```cpp
// Instantiating fooStructs
fooStruct myFooInstance("myReg"), myOtherFooInstance("myOtherReg");
```

One of these instances contains a register called *myReg* and the other one a register called *myOtherReg*. Let's give them some initial value before doing anything:

```cpp
// Directly modifying myFooInstance.reg value
myFooInstance.reg = 0x55555555; // 01010101010101010101010101010101
// Directly modifying myOtherFooInstance.reg value
myOtherFooInstance.reg = 0x33333333; // 00110011001100110011001100110011
```

Now we are going to register 2 callbacks to `myFooInstance.reg`

- A pre-read callback calling its own `checkSomethingBeforeReading(.)` method, and
- A post-write callback calling function `displayValuesAfterWriting(.)`.

The first registration is done the following way:

```cpp
// Registering checkSomethingBeforeReading(.) to myFooInstance.reg associated to myFooInstance.reg
myFooInstance.reg.registerPreReadCallback(std::bind(&fooStruct::checkSomethingBeforeReading, &myFooInstance, std::placeholders::_1));
```

The use of `std::bind(method, instance*, placeholders...)` creates a function pointer made by binding the given instance to the given class method, holding the parameters defined by the number of subsequent placeholders.

## Field-Level Read/Write Mode Definition - TODO