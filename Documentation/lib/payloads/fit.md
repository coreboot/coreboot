# Flattened uImage Tree documentation

[uImage.FIT] is the new format used for uImage payloads developed by
[U-boot].

## Supported architectures

* aarch64

## Supported FIT sections

The FIT can contain multiple sections, each holding a unique
kernel, initrd or config. Out of the sections one kernel and
one initrd is chosen, depending on the matching config.

The config is selected depending on the compat string.

The section must be named in order to be found by the FIT parser:

* kernel
* fdt
* ramdisk

## Architecture specifics

The FIT parser needs architecure support.
### aarch64
The source code can be found in ''src/arch/arm64/fit.c''.

On aarch64 the kernel (a section named 'kernel') must be in **Image**
format and it needs a devicetree (a section named 'fdt') to boot.
The kernel will be placed close to "*DRAMSTART*".

### Other
Other architectures aren't supported.

## Supported compression

The FIT image has to be included uncompressed into the CBFS. The sections
inside the FIT image can use different compression schemes.

Supported compression algorithms:
* LZMA
* LZ4
* none

## Compat string

The config entries contain a compatible string, that is used to find a
matching config.

The following mainboard specific funtions provide the BOARDID and SKUID:

```c
uint32_t board_id(void);
```

```c
uint32_t sku_id(void);
```

By default the following compat strings are added:

* *CONFIG_MAINBOARD_VENDOR*,*CONFIG_MAINBOARD_PART_NUMBER*
* *CONFIG_MAINBOARD_VENDOR*,*CONFIG_MAINBOARD_PART_NUMBER*-rev*BOARDID*
* *CONFIG_MAINBOARD_VENDOR*,*CONFIG_MAINBOARD_PART_NUMBER*-rev*BOARDID*-sku*SKUID*

Example:

```
cavium,cn8100_sff_evb
```

If *board_id()* or *sku_id()* return invalid, the single comapt string isn't added.

You can add custom compat strings by calling:

```c
fit_add_compat_string(const char *str);
```

If no matching compat string is found, the default config is chosen.

## Building FIT image

The FIT image has to be built by calling ''mkimage''. You can use
the following example configuration:

```
/*
 * Simple U-Boot uImage source file containing a single kernel and FDT blob
 */

/dts-v1/;

/ {
	description = "Simple image with single Linux kernel and FDT blob";
	#address-cells = <1>;

	images {
		kernel {
			description = "Vanilla Linux kernel";
			data = /incbin/("Image.lzma");
			type = "kernel";
			arch = "arm64";
			os = "linux";
			compression = "lzma";
			load = <0x80000>;
			entry = <0x80000>;
			hash-1 {
				algo = "crc32";
			};
		};
		fdt-1 {
			description = "Flattened Device Tree blob";
			data = /incbin/("target.dtb");
			type = "flat_dt";
			arch = "arm64";
			compression = "none";
			hash-1 {
				algo = "crc32";
			};
		};
		ramdisk-1 {
                        description = "Compressed Initramfs";
                        data = /incbin/("initramfs.cpio.xz");
                        type = "ramdisk";
                        arch = "arm64";
                        os = "linux";
                        compression = "none";
                        load = <00000000>;
                        entry = <00000000>;
                        hash-1 {
                                algo = "sha1";
                        };
		};
	};

	configurations {
		default = "conf-1";
		conf-1 {
			description = "Boot Linux kernel with FDT blob";
			kernel = "kernel";
			fdt = "fdt-1";
			ramdisk = "ramdisk-1";
		};
	};
};
```

It includes a compressed initrd **initramfs.cpio.xz**, which will be
decompressed by the Linux kernel, a compressed kernel **Image.lzma**, which will
be decompressed by the FIT loader and an uncompressed devicetree blob.

[uImage.FIT]: https://raw.githubusercontent.com/u-boot/u-boot/master/doc/uImage.FIT/howto.txt
[U-Boot]: https://www.denx.de/wiki/U-Boot
