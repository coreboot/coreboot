# Address Sanitizer

Memory safety is hard to achieve. We, as humans, are bound to make mistakes in
our code. While it may be straightforward to detect memory corruption bugs in
few lines of code, it becomes quite challenging to find those bugs in a massive
code. In such cases, 'Address Sanitizer' may prove to be useful and could help
save time.

[Address Sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)
, also known as ASan, is a runtime memory debugger designed to find
out-of-bounds accesses and use-after-scope bugs. coreboot has an in-built
Address Sanitizer. Therefore, it is advised to take advantage of this debugging
tool while working on large patches. This would further help to ensure code
quality and make runtime code more robust.

## Types of errors detected
ASan in coreboot catches the following types of memory bugs:

### Stack buffer overflow
Example stack-out-of-bounds:
```c
void foo()
{
	int stack_array[5] = {0};
        int i, out;
	for (i = 0; i < 10; i++)
		out = stack_array[i];
}
```
In this example, the array is of length 5 but it is being read even beyond the
index 4.

### Global buffer overflow
Example global-out-of-bounds:
```c
char a[] = "I use coreboot";

void foo()
{
        char b[] = "proprietary BIOS";
        strcpy(a + 6, b);
}
```
In this example,
> well, you are replacing coreboot with proprietary BIOS. In any case, that's
an "error".

Let's come to the memory bug. The string 'a' is of length 14 but it is being
written to even beyond that.

### Use after scope
Example use-after-scope:
```c
volatile int *p = 0;

void foo() {
  {
    int x = 0;
    p = &x;
  }
  *p = 5;
}
```
In this example, the value 5 is written to an undefined address instead of the
variable 'x'. This happens because 'x' can't be accessed outside its scope.

## Using ASan

In order to enable ASan on a supported platform,
select `Address sanitizer support` from `General setup` menu while configuring
coreboot.

Then build coreboot and run the image as usual. If your code contains any of the
above-mentioned memory bugs, ASan will report them in the console log as shown
below:
```text
ASan: <bug type> in <ip>
<access type> of <access size> bytes at addr <access address>
```
where,

`bug type` is either `stack-out-of-bounds`, `global-out-of-bounds` or
`use-after-scope`,

`ip` is the address of the last good instruction before the bad access,

`access type` is either `Read` or `Write`,

`access size` is the number of bytes read or written, and

`access address` is the memory location which is accessed while the error
occurs.

Next, you have to use `ip` to retrieve the instruction which causes the error.
Since stages in coreboot are relocated, you need to normalize `ip`. For this,
first subtract the start address of the stage from `ip`. Then, read the section
headers from `<stage>.debug` file to determine the offset of the text segment.
Add this offset to the difference you calculated earlier. Let's call the
resultant address `ip'`.

Next, read the contents of the symbol table and search for a function having
an address closest to `ip'`. This is the function in which our memory bug is
present. Let's denote the address of this function by `ip''`.

Finally, read the assembly contents of the object file where this function is
present. Look for the affected function. Here, the instruction which exists at
the offset `ip' - ip''` corresponds to the address `ip`. Therefore, the very
next instruction is the one which causes the error.

To see ASan in action, let's take an example. Suppose, there is a
stack-out-of-bounds error in cbfs.c that we aren’t aware of and we want ASan
to help us detect it.
```c
int cbfs_boot_region_device(struct region_device *rdev)
{
	int array[5], i;
	boot_device_init();

	for (i = 10; i > 0; i--)
		array[i] = i;

	return vboot_locate_cbfs(rdev) &&
	       fmap_locate_area_as_rdev("COREBOOT", rdev);
}
```
First, we enable ASan from the configuration menu as shown above. Then, we
build coreboot and run the image.

ASan reports the following error in the console log:
```text
ASan: stack-out-of-bounds in 0x7f7432fd
Write of 4 bytes at addr 0x7f7c2ac8
```
Here 0x7f7432fd is `ip` i.e. the address of the last good instruction before
the bad access. First we have to normalize this address as stated above.
As per the console log, this error happened in ramstage and the stage starts
from 0x7f72c000. So, let’s look at the sections headers of ramstage from
`ramstage.debug`.
```text
$ objdump -h build/cbfs/fallback/ramstage.debug

build/cbfs/fallback/ramstage.debug:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         00070b20  00e00000  00e00000  00001000  2**12
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .ctors        0000036c  00e70b20  00e70b20  00071b20  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
  2 .data         0001c8f4  00e70e8c  00e70e8c  00071e8c  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
  3 .bss          00012940  00e8d780  00e8d780  0008e780  2**7
                  ALLOC
  4 .heap         00004000  00ea00c0  00ea00c0  0008e780  2**0
                  ALLOC
```
As you can see, the offset of the text segment is 0x00e00000. Let's subtract the
start address of the stage from `ip` and add this offset to the difference. The
resultant address i.e. `ip'` is 0x00e172fd.

Next, we read the contents of the symbol table and search for a function having
an address closest to 0x00e172fd.
```text
$ nm -n build/cbfs/fallback/ramstage.debug
........
........
00e17116 t _GLOBAL__sub_I_65535_1_gfx_get_init_done
00e17129 t tohex16
00e171db T cbfs_load_and_decompress
00e1729b T cbfs_boot_region_device
00e17387 T cbfs_boot_locate
00e1740d T cbfs_boot_map_with_leak
00e174ef T cbfs_boot_map_optionrom
........
........
```
The symbol having an address closest to 0x00e172fd is `cbfs_boot_region_device` and
its address i.e. `ip''` is 0x00e1729b.

Now, as we know the affected function, let's read the assembly contents of
`cbfs_boot_region_device()` which is present in `cbfs.o` to find the faulty
instruction.
```text
$ objdump -d build/ramstage/lib/cbfs.o
........
........
  51:   e8 fc ff ff ff          call   52 <cbfs_boot_region_device+0x52>
  56:   83 ec 0c                sub    $0xc,%esp
  59:   57                      push   %edi
  5a:   83 ef 04                sub    $0x4,%edi
  5d:   e8 fc ff ff ff          call   5e <cbfs_boot_region_device+0x5e>
  62:   83 c4 10                add    $0x10,%esp
  65:   89 5f 04                mov    %ebx,0x4(%edi)
  68:   4b                      dec    %ebx
  69:   75 eb                   jne    56 <cbfs_boot_region_device+0x56>
........
........
```
Here, we look for the instruction present at the offset 62 i.e. `ip' - ip''`.
The instruction is `add $0x10,%esp` and it corresponds to
`for (i = 10; i > 0; i--)` in our code. It means the very next instruction
i.e. `mov %ebx,0x4(%edi)` is the one that causes the error. Now, as we look at
C code of `cbfs_boot_region_device()` again, we find that this instruction
corresponds to `array[i] = i`.

Voilà! We just caught the memory bug using ASan.

## Supported platforms
Presently, the following architectures support ASan in ramstage:
```eval_rst
+------------------+--------------------------------+
| Architecture     | Notes                          |
+==================+================================+
| x86              | Support for all x86 platforms  |
+------------------+--------------------------------+
```

And in romstage ASan is available on the following platforms:
```eval_rst
+---------------------+-----------------------------+
| Platform            | Notes                       |
+=====================+=============================+
| QEMU i440-fx        |                             |
+---------------------+-----------------------------+
| Intel Apollo Lake   |                             |
+---------------------+-----------------------------+
| Intel Haswell       |                             |
+---------------------+-----------------------------+
```
Alternatively, you can use `grep` to view the list of platforms that support
ASan in romstage:

    $ git grep "select HAVE_ASAN_IN_ROMSTAGE"

If the x86 platform you are using is not listed here, there is
still a chance that it supports ASan in romstage.

To test it, select `HAVE_ASAN_IN_ROMSTAGE` from the Kconfig file in the
platform's dedicated directory. Then, enable ASan from the config menu as
indicated in the previous section.

If you are able to build coreboot without any errors and boot cleanly, that
means the platform supports ASan in romstage. In that case, please upload a
patch on Gerrit selecting this config option using 'ASan' topic. Also, update
the platform name in the table.

However, if you end up in compilation errors or the linker error saying that
the cache got full, additional steps need to be taken to enable ASan in
romstage on the platform. While compile errors could be resolved easily and
therefore ASan in romstage has a good chance to be supported, a full cache is
an indication that it is way more work or even likely impossible to enable
ASan in romstage.

## Future work
### Heap buffer overflow
Presently, ASan doesn't detect out-of-bounds accesses for the objects defined
in heap.

To add support for these type of memory bugs, you have to make sure that
whenever some block of memory is allocated in the heap, the surrounding areas
(redzones) are poisoned. Correspondingly, these redzones should be unpoisoned
when the memory block is de-allocated.

### ASan on other architectures
The following points should help when adding support for ASan to other
architectures like ARM or RISC-V:

* Enabling ASan in ramstage on other architectures should be easy. You just
have to make sure the shadow memory is initialized as early as possible when
ramstage is loaded. This can be done by making a function call to `asan_init()`
at the appropriate place.

* For romstage, you have to find out if there is enough room in the cache to fit
the shadow memory region. For this, find the boundary linker symbols for the
region you'd want to run ASan on, excluding the hardware mapped addresses.
Then define a new linker section named `asan_shadow` of size
`(_end - _start) >> 3`, where `_start` and `_end` are the linker symbols you
found earlier. This section should be appended to the region already occupied
by the coreboot program. Now build coreboot. If you don't see any errors while
building with the current translation function, ASan can be enabled on that
platform.

* The shadow region we currently use consumes memory equal to 1/8th of the
program memory. So, if you end up in a linker error saying that the memory got
full, you'll have to use a more compact shadow region. In that case, the
translation function could be something like
`shadow = (mem >> 7) | shadow_offset`. Since the stack buffers are protected by
the compiler, you'll also have to create a GCC patch forcing it to use the new
translation function for this particular architecture.

* Once you are sure that the architecture supports ASan in ramstage, select
`HAVE_ASAN_IN_RAMSTAGE` from the Kconfig file of that architecture. Similarly,
if the platform supports ASan in romstage, select `HAVE_ASAN_IN_ROMSTAGE` from
the platform's dedicated Kconfig file.

### Post-processing script
Unlike Linux, coreboot doesn't have `%pS` printk format to dereference pointer
to its symbolic name. Therefore, we normalise the pointer address manually to
determine the name of the affected function and further use it to find the
instruction which causes the error.

A custom script can be written to automate this process.
